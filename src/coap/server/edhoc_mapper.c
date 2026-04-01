#include "coap/server/edhoc_mapper.h"

#include <stdlib.h>

coap_pdu_code_t coap_server_map_edhoc_failure_to_response(
    struct edhoc_context* edhoc_ctx, const char* operation_label,
    const coap_server_edhoc_failure_type_t failure_type,
    const int edhoc_api_result, coap_pdu_t* response, common_response_buffer_t* response_data) {
  const coap_pdu_code_t mapped_response_code =
      failure_type == COAP_SERVER_EDHOC_PROTOCOL_ERROR
          ? COAP_RESPONSE_CODE_BAD_REQUEST
          : COAP_RESPONSE_CODE_INTERNAL_ERROR;

  if (!response) {
    return mapped_response_code;
  }

  if (!edhoc_ctx || !response_data) {
    coap_log_err("cannot map EDHOC error without context or response data\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }

  enum edhoc_error_code edhoc_error_code = EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;
  if (edhoc_error_get_code(edhoc_ctx, &edhoc_error_code) != EDHOC_SUCCESS) {
    coap_log_err(
        "cannot get EDHOC-level error code, using UNSPECIFIED_ERROR\n");
  }

  coap_log_err("cannot %s, libedhoc ret: %d, edhoc error code: %d\n",
               operation_label, edhoc_api_result, edhoc_error_code);

  if (edhoc_message_error_compose(response_data->payload,
                                  response_data->payload_capacity,
                                  &response_data->payload_length,
                                  edhoc_error_code, NULL) != EDHOC_SUCCESS) {
    coap_log_err(
        "cannot compose EDHOC error message for client, using empty payload\n");
    response_data->payload_length = 0;
  }

  coap_pdu_set_code(response, mapped_response_code);
  return mapped_response_code;
}

/**
 * @brief Get CoAP response code corresponding to EDHOC server handshake status.
 * @param status EDHOC server handshake status.
 * @return CoAP response code
 */
static coap_pdu_code_t map_message_1_status_to_response(
    const edhoc_server_message_1_status_t status) {
  switch (status) {
  case CSH_OK:
    return COAP_RESPONSE_CODE_CHANGED;

  case CSH_ERR_PREFIX_MISSING:
  case CSH_ERR_INVALID_ARGS:
  case CSH_ERR_COAP_SESSION_ALREADY_HAS_DATA:
  case CSH_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED:
    return COAP_RESPONSE_CODE_BAD_REQUEST;

  case CSH_ERR_CALLOC_FAILED:
  case CSH_ERR_EDHOC_CONTEXT_SETUP_FAILED:
  case CSH_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED:
  default:
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
}

static void log_message_1_failure(
    const edhoc_server_message_1_status_t status) {
  static const char ERROR_PREFIX[] = "Message 1 processing failed: ";
  switch (status) {
  case CSH_ERR_INVALID_ARGS:
    coap_log_err("%s invalid arguments\n", ERROR_PREFIX);
    break;
  case CSH_ERR_PAYLOAD_TOO_LARGE:
    coap_log_err("%s payload exceeds maximum buffer size\n", ERROR_PREFIX);
    break;
  case CSH_ERR_PREFIX_MISSING:
    coap_log_err("%s missing CBOR true prefix\n", ERROR_PREFIX);
    break;
  case CSH_ERR_CALLOC_FAILED:
    coap_log_err("%s memory allocation failed\n", ERROR_PREFIX);
    break;
  case CSH_ERR_COAP_SESSION_ALREADY_HAS_DATA:
    coap_log_err("%s session already has EDHOC data\n", ERROR_PREFIX);
    break;
  case CSH_ERR_EDHOC_CONTEXT_SETUP_FAILED:
    coap_log_err("%s libedhoc context setup failed\n", ERROR_PREFIX);
    break;
  case CSH_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED:
    coap_log_err("%s libedhoc message_1_process failed\n", ERROR_PREFIX);
    break;
  case CSH_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED:
    coap_log_err("%s libedhoc message_2_compose failed\n", ERROR_PREFIX);
    break;
  case CSH_OK:
    coap_log_err(
        "%s THIS SHOULD NEVER HAPPEN: log failure called on success status\n",
        ERROR_PREFIX);
    break;
  default:
    coap_log_err("%s unknown error\n", ERROR_PREFIX);
  }
}

coap_pdu_code_t coap_server_process_message_1_result(
    const edhoc_server_message_1_result_t message_1_result,
    coap_session_t* session) {
  if (message_1_result.status != CSH_OK) {
    log_message_1_failure(message_1_result.status);
    return map_message_1_status_to_response(message_1_result.status);
  }
  if (coap_session_set_app_data2(session, message_1_result.edhoc_ctx, free) !=
      NULL) {
    coap_log_err("already existing EDHOC context in CoAP session app data\n");
    free(message_1_result.edhoc_ctx);
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
  return map_message_1_status_to_response(message_1_result.status);
}
