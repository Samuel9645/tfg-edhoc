#include "srv_dispatch_engine.h"

#include "coap/coap_config.h"
#include "coap/common/com_coap_parse_edhoc_request.h"
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
         deps->respond_to_message_1 != NULL &&
         deps->process_message_1_result != NULL &&
         deps->respond_to_message_3 != NULL &&
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

static bool add_payload_if_present(
    coap_pdu_t* response, const struct com_readonly_buffer payload,
    const srv_coap_add_response_payload_fn add_payload_fn) {
  if (payload.length > 0 && add_payload_fn(response, payload.bytes,
                                           payload.length) != STATUS_COAP_OK) {
    coap_log_err("failed to add response payload\n");
    return false;
  }
  return true;
}

// TODO: maybe only pass the specific deps instead of all
static coap_pdu_code_t route_and_process_edhoc_message(
    coap_session_t* session, const struct com_readonly_buffer parsed_request,
    const struct srv_edhoc_parameters edhoc_parameters, coap_pdu_t* response,
    const struct srv_coap_dispatch_deps* deps) {
  uint8_t response_payload[CONFIG_COAP_MAX_PDU_SIZE] = {0};
  const struct com_writable_buffer response_buffer = {
      .bytes = response_payload,
      .capacity = CONFIG_COAP_MAX_PDU_SIZE,
  };
  struct edhoc_context* edhoc_ctx = deps->get_session_app_data(session);
  if (edhoc_ctx == NULL) {
    const struct srv_edhoc_message_1_responder_request request = {
        .raw_payload = parsed_request};
    const struct srv_edhoc_message_1_responder_result message_1_result =
        deps->respond_to_message_1(request, edhoc_parameters, response_buffer);
    if (!add_payload_if_present(response, message_1_result.response,
                                deps->add_response_payload)) {
      return COAP_RESPONSE_CODE_INTERNAL_ERROR;
    }
    return deps->process_message_1_result(message_1_result, session);
  }

  const struct srv_edhoc_message_3_responder_request handler_request = {
      .edhoc_context = edhoc_ctx, .raw_payload = parsed_request};
  const struct srv_edhoc_message_3_responder_result message_3_result =
      deps->respond_to_message_3(handler_request, response_buffer);
  if (!add_payload_if_present(response, message_3_result.response,
                              deps->add_response_payload)) {
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
  return deps->process_message_3_result(message_3_result);
}

void srv_coap_dispatch_post_with_dependencies(
    coap_session_t* session, const coap_pdu_t* request,
    const struct srv_edhoc_parameters edhoc_parameters, coap_pdu_t* response,
    const struct srv_coap_dispatch_deps* deps) {
  if (srv_dispatch_has_invalid_deps_or_args(session, request, response, deps)) {
    coap_log_err("FATAL: Missing dependencies in dispatcher!\n");
    if (response != NULL) {
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    }
    return;
  }

  uint8_t pdu_buffer[CONFIG_COAP_MAX_PDU_SIZE] = {0};
  const struct com_writable_buffer pdu_data = {
      .bytes = pdu_buffer,
      .capacity = CONFIG_COAP_MAX_PDU_SIZE,
  };
  const struct com_coap_parse_edhoc_request_result parse_edhoc_result =
      deps->parse_edhoc_request(request, CONFIG_COAP_CONTENT_CID_EDHOC,
                                pdu_data);
  if (parse_edhoc_result.status != COM_COAP_EDH_REQ_OK) {
    coap_log_err("failed to parse EDHOC message\n");
    coap_pdu_set_code(response, com_coap_map_parse_result_to_pdu_code(
                                    parse_edhoc_result.status));
    return;
  }
  if (deps->add_edhoc_response_options(response, CONFIG_COAP_CONTENT_EDHOC) !=
      STATUS_COAP_OK) {
    coap_log_err("failed to add EDHOC response options\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }
  coap_pdu_set_code(response, route_and_process_edhoc_message(
                                  session, parse_edhoc_result.parsed_request,
                                  edhoc_parameters, response, deps));
}
