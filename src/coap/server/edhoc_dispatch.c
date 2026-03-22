#include "coap/server/edhoc_dispatch.h"

#include <edhoc_helpers.h>

#include "coap/common/config.h"
#include "coap/shared/edhoc_request.h"
#include "coap/shared/edhoc_response.h"
#include "edhoc/common/config.h"
#include "edhoc/server/server_flow.h"
#include "internal/edhoc_dispatch_internal.h"

const coap_server_edhoc_dispatch_deps_t
    COAP_SERVER_EDHOC_DISPATCH_DEFAULT_DEPS = {
        .validate_edhoc_request = coap_shared_validate_edhoc_request,
        .add_edhoc_response_options = coap_shared_add_edhoc_response_options,
        .is_message_1 = coap_shared_is_properly_formatted_message_1,
        .is_message_3 = coap_shared_is_properly_formatted_message_3,
        .handle_message_1 = server_edhoc_handle_message_1,
        .handle_message_3 = server_edhoc_handle_message_3,
        .add_response_payload = coap_shared_add_response_payload,
};

static inline bool coap_server_dispatch_has_invalid_deps_or_args(
    const coap_session_t* session, const coap_pdu_t* request,
    const coap_pdu_t* response, const coap_server_edhoc_dispatch_deps_t* deps) {
  return !session || !request || !response || !deps ||
         !deps->validate_edhoc_request || !deps->add_edhoc_response_options ||
         !deps->is_message_1 || !deps->is_message_3 ||
         !deps->handle_message_1 || !deps->handle_message_3 ||
         !deps->add_response_payload;
}

void coap_server_dispatch_edhoc_post(coap_session_t* session,
                                     const coap_pdu_t* request,
                                     coap_pdu_t* response) {
  coap_server_dispatch_edhoc_post_with_deps(
      session, request, response, &COAP_SERVER_EDHOC_DISPATCH_DEFAULT_DEPS);
}

void coap_server_dispatch_edhoc_post_with_deps(
    coap_session_t* session, const coap_pdu_t* request, coap_pdu_t* response,
    const coap_server_edhoc_dispatch_deps_t* deps) {
  if (coap_server_dispatch_has_invalid_deps_or_args(session, request, response,
                                                    deps)) {
    coap_log_err("invalid arguments passed to EDHOC dispatcher\n");
    if (response) {
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    }
    return;
  }

  const uint8_t* request_payload = NULL;
  size_t request_len = 0;
  if (deps->validate_edhoc_request(request, APPLICATION_CID_EDHOC_CBOR_SEQ,
                                   &request_payload,
                                   &request_len) != COAP_STATUS_SUCCESS) {
    coap_log_err("failed to validate EDHOC request\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
    return;
  }

  if (deps->add_edhoc_response_options(response, APPLICATION_EDHOC_CBOR_SEQ) !=
      COAP_STATUS_SUCCESS) {
    coap_log_err("failed to add EDHOC response options\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  uint8_t response_payload[MESSAGE_BUFFER_LENGTH] = {0};
  size_t response_len = 0;

  coap_response_data_t response_data = {
      .payload = response_payload,
      .payload_capacity = MESSAGE_BUFFER_LENGTH,
      .payload_len = &response_len,
  };

  coap_pdu_code_t response_code = COAP_RESPONSE_CODE_INTERNAL_ERROR;
  struct edhoc_extracted_fields message_3_extracted_fields = {0};

  struct edhoc_context* edhoc_ctx =
      (struct edhoc_context*)coap_session_get_app_data(session);

  if (deps->is_message_1(request_payload, request_len)) {
    if (edhoc_ctx != NULL) {
      coap_log_err("EDHOC context already exists for this session\n");
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
      return;
    }

    server_edhoc_message_1_request_data_t request_data = {
        .base_data =
            {
                .session = session,
                .edhoc_ctx = edhoc_ctx,
                .response = response,
                .request_data =
                    {
                        .payload = request_payload,
                        .payload_len = request_len,
                    },
            },
    };
    response_code = deps->handle_message_1(&request_data, &response_data);
  } else if (edhoc_ctx == NULL ||
             deps->is_message_3(request_payload, request_len, edhoc_ctx,
                                &message_3_extracted_fields)) {
    if (edhoc_ctx == NULL) {
      coap_log_err("received Message 3 without EDHOC context\n");
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
      return;
    }

    server_edhoc_message_3_request_data_t request_data = {
        .base_data =
            {
                .session = session,
                .edhoc_ctx = edhoc_ctx,
                .response = response,
                .request_data =
                    {
                        .payload = request_payload,
                        .payload_len = request_len,
                    },
            },
        .message_3_extracted_fields = &message_3_extracted_fields,
    };
    response_code = deps->handle_message_3(&request_data, &response_data);
  } else {
    coap_log_err("received invalid or unexpected EDHOC message\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  if (response_len > 0 &&
      deps->add_response_payload(response, response_payload, response_len) !=
          COAP_STATUS_SUCCESS) {
    coap_log_err("failed to add response payload\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  coap_pdu_set_code(response, response_code);
}
