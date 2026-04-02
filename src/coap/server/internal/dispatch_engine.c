#include "dispatch_engine.h"

#include "coap/config.h"
#include "edhoc/config.h"

/**
 * @brief Validate all required dependency function pointers are non-NULL.
 *
 * @param[in] deps Dispatch dependencies structure.
 * @return true if all function pointers are present, false if any are NULL.
 */
static bool dispatch_deps_are_valid(const cp_serv_dispatch_deps_t* deps) {
  return deps != NULL && deps->extract_payload_if_valid_edhoc_request != NULL &&
         deps->add_edhoc_response_options != NULL &&
         deps->parse_message_1 != NULL &&
         deps->process_message_1_result != NULL &&
         deps->parse_message_3 != NULL && deps->handle_message_1 != NULL &&
         deps->handle_message_3 != NULL &&
         deps->process_message_3_result != NULL &&
         deps->add_response_payload != NULL &&
         deps->get_session_app_data != NULL;
}

static bool dispatch_has_invalid_deps_or_args(
    const coap_session_t* session, const coap_pdu_t* request,
    const coap_pdu_t* response, const cp_serv_dispatch_deps_t* deps) {
  return !session || !request || !response || !dispatch_deps_are_valid(deps);
}

void cp_srv_dispatch_post_with_dependencies(
    coap_session_t* session, const coap_pdu_t* request,
    const struct edhoc_credentials* credentials, coap_pdu_t* response,
    const cp_serv_dispatch_deps_t* deps) {
  if (dispatch_has_invalid_deps_or_args(session, request, response, deps)) {
    coap_log_err("FATAL: Missing dependencies in dispatcher!\n");
    if (response) {
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    }
    return;
  }

  const uint8_t* request_payload = NULL;
  size_t request_len = 0;
  if (deps->extract_payload_if_valid_edhoc_request(
          request, CP_CFG_CONTENT_CID_EDHOC, &request_payload, &request_len) !=
      CP_STATUS_SUCCESS) {
    coap_log_err("failed to validate EDHOC request\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
    return;
  }

  if (deps->add_edhoc_response_options(response, CP_CFG_CONTENT_EDHOC) !=
      CP_STATUS_SUCCESS) {
    coap_log_err("failed to add EDHOC response options\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  uint8_t response_payload[EDH_CFG_MESSAGE_BUFFER_LENGTH] = {0};

  com_response_buffer_t response_data = {
      .buffer = response_payload,
      .capacity = EDH_CFG_MESSAGE_BUFFER_LENGTH,
      .length = 0,
  };

  coap_pdu_code_t response_code = COAP_RESPONSE_CODE_INTERNAL_ERROR;
  com_request_payload_t message_1_parsed_payload = {0};
  struct edhoc_extracted_fields message_3_extracted_fields = {0};

  struct edhoc_context* edhoc_ctx = deps->get_session_app_data(session);

  if (deps->parse_message_1(request_payload, request_len,
                            &message_1_parsed_payload)) {
    if (edhoc_ctx != NULL) {
      coap_log_err("EDHOC context already exists for this session\n");
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
      return;
    }

    const edh_srv_message_1_request_t request_data = {
        .payload = message_1_parsed_payload, .credentials = credentials};

    const ehd_message_1_handler_result_t message_1_result =
        deps->handle_message_1(&request_data, &response_data);
    response_code = deps->process_message_1_result(message_1_result, session);
  } else if (deps->parse_message_3(request_payload, request_len, edhoc_ctx,
                                   &message_3_extracted_fields)) {
    if (edhoc_ctx == NULL) {
      coap_log_err("received Message 3 without EDHOC context\n");
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
      return;
    }

    const edh_srv_message_3_request_t request_data = {
        .base_data =
            {
                .session = session,
                .edhoc_ctx = edhoc_ctx,
                .response = response,
                .request_data =
                    {
                        .buffer = request_payload,
                        .length = request_len,
                    },
            },
        .message_3_extracted_fields = &message_3_extracted_fields,
    };
    const edh_message_3_handler_status_t message_3_result =
        deps->handle_message_3(&request_data, &response_data);
    response_code = deps->process_message_3_result(message_3_result);
  } else {
    coap_log_err("received invalid or unexpected EDHOC message\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_BAD_REQUEST);
    return;
  }

  if (response_data.length > 0 &&
      deps->add_response_payload(response, response_payload,
                                 response_data.length) != CP_STATUS_SUCCESS) {
    coap_log_err("failed to add response payload\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  coap_pdu_set_code(response, response_code);
}
