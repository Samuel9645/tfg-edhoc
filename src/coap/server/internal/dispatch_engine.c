#include "dispatch_engine.h"

#include "coap/coap_config.h"
#include "coap/server/map_error_to_response.h"
#include "edhoc/edhoc_config.h"

static inline bool coap_server_dispatch_has_invalid_deps_or_args(
    const coap_session_t* session, const coap_pdu_t* request,
    const coap_pdu_t* response, const coap_server_dispatch_deps_t* deps) {
  return !session || !request || !response || !deps ||
         !deps->extract_payload_if_valid_edhoc_request ||
         !deps->add_edhoc_response_options || !deps->is_message_1 ||
         !deps->is_message_3 || !deps->handle_message_1 ||
         !deps->handle_message_3 || !deps->add_response_payload ||
         !deps->get_session_app_data;
}

void coap_server_dispatch_post_with_dependencies(
    coap_session_t* session, const coap_pdu_t* request, coap_pdu_t* response,
    const coap_server_dispatch_deps_t* deps) {
  if (coap_server_dispatch_has_invalid_deps_or_args(session, request, response,
                                                    deps)) {
    coap_log_err("FATAL: Missing dependencies in dispatcher!\n");
    if (response) {
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    }
    return;
  }

  const uint8_t* request_payload = NULL;
  size_t request_len = 0;
  if (deps->extract_payload_if_valid_edhoc_request(
          request, APPLICATION_CID_EDHOC_CBOR_SEQ, &request_payload,
          &request_len) != CCOM_STATUS_SUCCESS) {
    coap_log_err("failed to validate EDHOC request\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
    return;
  }

  if (deps->add_edhoc_response_options(response, APPLICATION_EDHOC_CBOR_SEQ) !=
      CCOM_STATUS_SUCCESS) {
    coap_log_err("failed to add EDHOC response options\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  uint8_t response_payload[EDC_MESSAGE_BUFFER_LENGTH] = {0};

  common_response_buffer_t response_data = {
      .payload = response_payload,
      .payload_capacity = EDC_MESSAGE_BUFFER_LENGTH,
      .payload_len = 0,
  };

  coap_pdu_code_t response_code = COAP_RESPONSE_CODE_INTERNAL_ERROR;
  struct edhoc_extracted_fields message_3_extracted_fields = {0};

  struct edhoc_context* edhoc_ctx =
      (struct edhoc_context*)deps->get_session_app_data(session);

  if (deps->is_message_1(request_payload, request_len)) {
    if (edhoc_ctx != NULL) {
      coap_log_err("EDHOC context already exists for this session\n");
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
      return;
    }

    edhoc_server_common_request_data_t request_data = {
        .session = session,
        .edhoc_ctx = edhoc_ctx,
        .response = response,
        .request_data = {
            .payload = request_payload,
            .payload_len = request_len,
        }};

    edhoc_server_handshake_status_t status =
        deps->handle_message_1(&request_data, &response_data);
    response_code = coap_server_map_message_1_status_to_response(status);
  } else if (deps->is_message_3(request_payload, request_len, edhoc_ctx,
                                &message_3_extracted_fields)) {
    if (edhoc_ctx == NULL) {
      coap_log_err("received Message 3 without EDHOC context\n");
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
      return;
    }

    edhoc_server_message_3_request_data_t request_data = {
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
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
    return;
  }

  if (response_data.payload_len > 0 &&
      deps->add_response_payload(response, response_payload,
                                 response_data.payload_len) !=
          CCOM_STATUS_SUCCESS) {
    coap_log_err("failed to add response payload\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  coap_pdu_set_code(response, response_code);
}
