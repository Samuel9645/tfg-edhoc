#include "coap/server/internal/edhoc_dispatch_engine.h"

#include "coap/common/config.h"
#include "edhoc/common/config.h"

static inline bool coap_server_dispatch_has_invalid_deps_or_args(
    const coap_session_t* session, const coap_pdu_t* request,
    const coap_pdu_t* response, const coap_server_edhoc_dispatch_deps_t* deps) {
  return !session || !request || !response || !deps ||
         !deps->extract_payload_if_valid_edhoc_request ||
         !deps->add_edhoc_response_options || !deps->is_message_1 ||
         !deps->is_message_3 || !deps->handle_message_1 ||
         !deps->handle_message_3 || !deps->add_response_payload ||
         !deps->get_session_app_data || !deps->set_response_code ||
         !deps->log_error;
}

void coap_server_dispatch_edhoc_post_with_deps(
    coap_session_t* session, const coap_pdu_t* request, coap_pdu_t* response,
    const coap_server_edhoc_dispatch_deps_t* deps) {
  if (coap_server_dispatch_has_invalid_deps_or_args(session, request, response,
                                                    deps)) {
    if (response) {
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    }
    return;
  }

  const uint8_t* request_payload = NULL;
  size_t request_len = 0;
  if (deps->extract_payload_if_valid_edhoc_request(
          request, APPLICATION_CID_EDHOC_CBOR_SEQ, &request_payload,
          &request_len) != COAP_STATUS_SUCCESS) {
    deps->log_error("failed to validate EDHOC request\n");
    deps->set_response_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
    return;
  }

  if (deps->add_edhoc_response_options(response, APPLICATION_EDHOC_CBOR_SEQ) !=
      COAP_STATUS_SUCCESS) {
    deps->log_error("failed to add EDHOC response options\n");
    deps->set_response_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
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
      (struct edhoc_context*)deps->get_session_app_data(session);

  if (deps->is_message_1(request_payload, request_len)) {
    if (edhoc_ctx != NULL) {
      deps->log_error("EDHOC context already exists for this session\n");
      deps->set_response_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
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
  } else if (deps->is_message_3(request_payload, request_len, edhoc_ctx,
                                &message_3_extracted_fields)) {
    if (edhoc_ctx == NULL) {
      deps->log_error("received Message 3 without EDHOC context\n");
      deps->set_response_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
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
    deps->log_error("received invalid or unexpected EDHOC message\n");
    deps->set_response_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  if (response_len > 0 &&
      deps->add_response_payload(response, response_payload, response_len) !=
          COAP_STATUS_SUCCESS) {
    deps->log_error("failed to add response payload\n");
    deps->set_response_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  deps->set_response_code(response, response_code);
}
