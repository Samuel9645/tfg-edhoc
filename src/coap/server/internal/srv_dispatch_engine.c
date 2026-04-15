#include "srv_dispatch_engine.h"

#include "coap/coap_config.h"
#include "coap/server/srv_parse_edhoc_request.h"
#include "edhoc/edhoc_config.h"

/**
 * @brief Validate all required dependency function pointers are non-NULL.
 *
 * @param[in] deps Dispatch dependencies structure.
 * @return true if all function pointers are present, false if any are NULL.
 */
static bool dispatch_deps_are_valid(const struct srv_coap_dispatch_deps* deps) {
  return deps != NULL && deps->parse_edhoc_request != NULL &&
         deps->add_edhoc_response_options != NULL &&
         deps->parse_message_1 != NULL &&
         deps->process_message_1_result != NULL &&
         deps->parse_message_3 != NULL && deps->handle_message_1 != NULL &&
         deps->handle_message_3 != NULL &&
         deps->process_message_3_result != NULL &&
         deps->add_response_payload != NULL &&
         deps->get_session_app_data != NULL;
}

static bool srv_dispatch_has_invalid_deps_or_args(
    const coap_session_t* session, const coap_pdu_t* request,
    const coap_pdu_t* response, const struct srv_coap_dispatch_deps* deps) {
  return session == NULL || request == NULL || response == NULL ||
         !dispatch_deps_are_valid(deps);
}

static coap_pdu_code_t map_parse_message_1_status_to_pdu_code(
    const enum srv_edhoc_parse_message_1_status status) {
  switch (status) {
  case SRV_EDHOC_MSG1_PARSE_ERR_PREFIX_EXTRACTION:
    return COAP_RESPONSE_CODE_BAD_REQUEST;
  case SRV_EDHOC_MSG1_PARSE_ERR_INVALID_REQUEST_BUFFER:
  default:
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
}

coap_pdu_code_t map_parse_message_3_status_to_pdu_code(
    const enum srv_edhoc_parse_message_3_status status) {
  switch (status) {
  case SRV_EDHOC_MSG3_PARSE_ERR_CON_ID_EXTRACTION_FAILED:
  case SRV_EDHOC_MSG3_PARSE_ERR_UNEXPECTED_CONNECTION_ID:
    return COAP_RESPONSE_CODE_BAD_REQUEST;
  case SRV_EDHOC_MSG3_PARSE_ERR_INVALID_REQUEST_BUFFER:
  default:
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
}

void srv_coap_dispatch_post_with_dependencies(
    coap_session_t* session, const coap_pdu_t* request,
    const struct edhoc_credentials* credentials, coap_pdu_t* response,
    const struct srv_coap_dispatch_deps* deps) {
  if (srv_dispatch_has_invalid_deps_or_args(session, request, response, deps)) {
    coap_log_err("FATAL: Missing dependencies in dispatcher!\n");
    if (response) {
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    }
    return;
  }

  uint8_t pdu_buffer[CONFIG_COAP_MAX_PDU_SIZE] = {0};
  struct com_writable_buffer pdu_data = {
      .bytes = pdu_buffer,
      .capacity = CONFIG_COAP_MAX_PDU_SIZE,
      .length = 0,
  };
  const struct srv_coap_parse_edhoc_request_result parse_edhoc_result =
      deps->parse_edhoc_request(request, CONFIG_COAP_CONTENT_CID_EDHOC,
                                &pdu_data);
  if (parse_edhoc_result.status != SRV_COAP_EDH_REQ_OK) {
    coap_log_err("failed to parse EDHOC message\n");
    coap_pdu_set_code(response, srv_coap_map_parse_result_to_pdu_code(
                                    parse_edhoc_result.status));
    return;
  }
  if (deps->add_edhoc_response_options(response, CONFIG_COAP_CONTENT_EDHOC) !=
      STATUS_COAP_OK) {
    coap_log_err("failed to add EDHOC response options\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }
  uint8_t response_payload[CONFIG_COAP_MAX_PDU_SIZE] = {0};
  struct com_writable_buffer response_data = {
      .bytes = response_payload,
      .capacity = CONFIG_COAP_MAX_PDU_SIZE,
      .length = 0,
  };
  coap_pdu_code_t response_code = COAP_RESPONSE_CODE_INTERNAL_ERROR;
  struct edhoc_context* edhoc_ctx = deps->get_session_app_data(session);
  if (edhoc_ctx == NULL) {
    const struct srv_edhoc_parse_message_1_result parse_message_1_result =
        deps->parse_message_1(parse_edhoc_result.parsed_request);
    if (parse_message_1_result.status != SRV_EDHOC_MSG1_PARSE_OK) {
      coap_log_err("failed to parse Message 1: %s\n",
                   srv_edhoc_parse_message_1_status_to_string(
                       parse_message_1_result.status));
      coap_pdu_set_code(response, map_parse_message_1_status_to_pdu_code(
                                      parse_message_1_result.status));
      return;
    }
    const struct srv_edhoc_message_1_request request_data = {
        .payload = parse_message_1_result.parsed_message_1,
        .credentials = credentials};
    const struct srv_edhoc_message_1_handler_result message_1_result =
        deps->handle_message_1(request_data, &response_data);
    response_code = deps->process_message_1_result(message_1_result, session);
  } else {
    const struct srv_edhoc_parse_message_3_result parse_message_3_result =
        deps->parse_message_3(parse_edhoc_result.parsed_request, edhoc_ctx);
    if (parse_message_3_result.status != SRV_EDHOC_MSG3_PARSE_OK) {
      coap_log_err("failed to parse Message 3: %s\n",
                   srv_edhoc_parse_message_3_status_to_string(
                       parse_message_3_result.status));
      coap_pdu_set_code(response, map_parse_message_3_status_to_pdu_code(
                                      parse_message_3_result.status));
      return;
    }
    const struct srv_edhoc_message_3_request handler_request = {
        .edhoc_context = edhoc_ctx,
        .parsed_message_3 = parse_message_3_result.parsed_message_3};
    const enum srv_edhoc_message_3_handler_status message_3_result =
        deps->handle_message_3(handler_request, &response_data);
    response_code = deps->process_message_3_result(message_3_result);
  }
  if (response_data.length > 0 &&
      deps->add_response_payload(response, response_payload,
                                 response_data.length) != STATUS_COAP_OK) {
    coap_log_err("failed to add response payload\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  coap_pdu_set_code(response, response_code);
}
