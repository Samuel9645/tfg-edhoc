#include "srv_dispatch_engine.h"

#include <stdlib.h>

#include "coap/coap_config.h"
#include "coap/common/com_coap_parse_edhoc_request.h"
#include "coap/server/extract_edhoc_message/srv_coap_extract_m1.h"
#include "coap/server/extract_edhoc_message/srv_coap_extract_m3.h"
#include "coap/server/oscore/srv_oscore_bind_session.h"
#include "edhoc/common/com_edhoc_setup_context.h"
#include "edhoc/server/handshake/message_1/srv_m1_process.h"

/**
 * @brief Validate all required dependency function pointers are non-NULL.
 *
 * @param[in] deps Dispatch dependencies structure.
 * @return true if all function pointers are present, false if any are NULL.
 */
static bool dispatch_deps_are_valid(const struct srv_coap_dispatch_deps* deps) {
  return deps != NULL && deps->parse_edhoc_request != NULL &&
         deps->add_edhoc_response_options != NULL &&
         deps->is_message_1 != NULL && deps->extract_message_1 != NULL &&
         deps->extract_cid != NULL && deps->connection_id_is_expected != NULL &&
         deps->respond_to_message_1 != NULL &&
         deps->process_message_1_result != NULL &&
         deps->respond_to_message_3 != NULL &&
         deps->process_message_3_result != NULL &&
         deps->add_response_payload != NULL &&
         deps->set_context_by_cid != NULL && deps->get_context_by_cid != NULL &&
         deps->remove_context_by_cid != NULL &&
         deps->bind_oscore_session != NULL;
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

static coap_pdu_code_t route_and_process_edhoc_message(
    const struct com_readonly_buffer parsed_request,
    const struct com_edhoc_parameters edhoc_parameters, coap_pdu_t* response,
    const struct srv_coap_dispatch_deps* deps, coap_context_t* context) {
  uint8_t response_payload[CONFIG_COAP_MAX_PDU_SIZE] = {0};
  const struct com_writable_buffer response_buffer = {
      .bytes = response_payload,
      .capacity = CONFIG_COAP_MAX_PDU_SIZE,
  };

  if (deps->is_message_1(parsed_request)) {
    struct edhoc_context* edhoc_context =
        calloc(1, sizeof(struct edhoc_context));
    if (edhoc_context == NULL) {
      coap_log_err("failed to allocate EDHOC context\n");
      return COAP_RESPONSE_CODE_INTERNAL_ERROR;
    }

    const struct com_edhoc_setup_context_result setup_context_result =
        com_edhoc_setup_context(edhoc_context, edhoc_parameters);
    if (setup_context_result.status != COM_EDHOC_SETUP_CTX_OK) {
      coap_log_err("failed to setup EDHOC context\n");
      srv_edhoc_cleanup_context(edhoc_context);
      return COAP_RESPONSE_CODE_INTERNAL_ERROR;
    }
    const struct srv_coap_extract_message_1_result parsed_message_1 =
        deps->extract_message_1(parsed_request, response_buffer);
    if (parsed_message_1.status != SRV_COAP_EXTRACT_MSG1_OK) {
      coap_log_err("failed to parse Message 1\n");
      srv_edhoc_cleanup_context(edhoc_context);
      return srv_coap_map_extract_message_1_to_pdu_code(
          parsed_message_1.status);
    }

    const struct srv_edhoc_message_1_responder_result message_1_result =
        deps->respond_to_message_1(parsed_message_1.buffer, edhoc_context,
                                   response_buffer);
    if (!add_payload_if_present(response, message_1_result.response,
                                deps->add_response_payload)) {
      srv_edhoc_cleanup_context(edhoc_context);
      return COAP_RESPONSE_CODE_INTERNAL_ERROR;
    }
    const coap_pdu_code_t process_m1_code =
        deps->process_message_1_result(message_1_result);
    if (process_m1_code != COAP_RESPONSE_CODE_CHANGED) {
      srv_edhoc_cleanup_context(edhoc_context);
      return process_m1_code;
    }
    if (deps->set_context_by_cid(&edhoc_context->private_cid, edhoc_context) !=
        SRV_SESSION_SET_OK) {
      coap_log_err("failed to store context in session dictionary\n");
      srv_edhoc_cleanup_context(edhoc_context);
      return COAP_RESPONSE_CODE_INTERNAL_ERROR;
    }
    return process_m1_code;
  }
  const struct srv_coap_extract_connection_id_result
      extract_connection_id_result = deps->extract_cid(parsed_request);
  if (extract_connection_id_result.status != SRV_COAP_EXTRACT_CID_OK) {
    coap_log_err("failed to extract Message 3 connection ID\n");
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }

  const struct edhoc_connection_id extracted_cid =
      extract_connection_id_result.cid;
  const struct srv_session_get_result get_result =
      deps->get_context_by_cid(&extracted_cid);
  if (get_result.status != SRV_SESSION_GET_OK) {
    coap_log_err("context not found in session dictionary\n");
    return COAP_RESPONSE_CODE_BAD_REQUEST;
  }
  struct edhoc_context* edhoc_context = get_result.context;
  if (!deps->connection_id_is_expected(&extracted_cid, edhoc_context)) {
    coap_log_err("unexpected Message 3 connection ID\n");
    deps->remove_context_by_cid(&extracted_cid);
    srv_edhoc_cleanup_context(edhoc_context);
    return COAP_RESPONSE_CODE_BAD_REQUEST;
  }

  const struct srv_edhoc_message_3_responder_request handler_request = {
      .edhoc_context = edhoc_context,
      .message_3 = extract_connection_id_result.message_payload,
  };
  const struct srv_edhoc_message_3_responder_result message_3_result =
      deps->respond_to_message_3(handler_request, response_buffer);
  if (!add_payload_if_present(response, message_3_result.response,
                              deps->add_response_payload)) {
    deps->remove_context_by_cid(&extracted_cid);
    srv_edhoc_cleanup_context(edhoc_context);
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }

  const coap_pdu_code_t process_message_3_code =
      deps->process_message_3_result(message_3_result);
  if (process_message_3_code != COAP_RESPONSE_CODE_CHANGED) {
    coap_log_err("EDHOC Message 3 failure: Removing context from dictionary\n");
    deps->remove_context_by_cid(&extracted_cid);
    srv_edhoc_cleanup_context(edhoc_context);
    return process_message_3_code;
  }
  const enum status_coap bind_status =
      deps->bind_oscore_session(context, edhoc_context);
  deps->remove_context_by_cid(&extracted_cid);
  srv_edhoc_cleanup_context(edhoc_context);
  if (bind_status != STATUS_COAP_OK) {
    coap_log_err("failed to bind OSCORE session\n");
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
  return process_message_3_code;
}

void srv_coap_dispatch_post_with_dependencies(
    const coap_pdu_t* request,
    const struct com_edhoc_parameters edhoc_parameters, coap_pdu_t* response,
    const struct srv_coap_dispatch_deps* deps, coap_context_t* context) {
  if (!dispatch_deps_are_valid(deps)) {
    coap_log_err("FATAL: Missing dependencies in dispatcher!\n");
    if (response != NULL) {
      coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    }
    return;
  }
  if (request == NULL || response == NULL || context == NULL) {
    coap_log_err(
        "Invalid arguments: request, response, and session must be "
        "non-NULL\n");
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
  if (parse_edhoc_result.status != COM_COAP_PARSE_EDHOC_REQ_OK) {
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
                                  parse_edhoc_result.parsed_request,
                                  edhoc_parameters, response, deps, context));
}
