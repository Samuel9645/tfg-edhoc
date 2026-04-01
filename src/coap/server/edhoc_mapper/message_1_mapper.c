/**
 * @file message_1_mapper.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief CoAP adapter implementation that maps Message 1 EDHOC result codes
 * into transport responses.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/server/edhoc_mapper/message_1_mapper.h"

#include <stdlib.h>

static coap_pdu_code_t map_message_1_status_to_response(
    const enum edh_srv_hnd_m1_status status) {
  switch (status) {
  case EDH_SERV_HND_M1_OK:
    return COAP_RESPONSE_CODE_CHANGED;

  case EDH_SERV_HND_M1_ERR_PREFIX_MISSING:
  case EDH_SERV_HND_M1_ERR_INVALID_ARGS:
  case EDH_SERV_HND_M1_ERR_COAP_SESSION_ALREADY_HAS_DATA:
  case EDH_SERV_HND_M1_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED:
    return COAP_RESPONSE_CODE_BAD_REQUEST;

  case EDH_SERV_HND_M1_ERR_CALLOC_FAILED:
  case EDH_SERV_HND_M1_ERR_EDHOC_CONTEXT_SETUP_FAILED:
  case EDH_SERV_HND_M1_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED:
  default:
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
}

static const char* message_1_status_to_error_string(
    const enum edh_srv_hnd_m1_status status) {
  switch (status) {
  case EDH_SERV_HND_M1_ERR_INVALID_ARGS:
    return "invalid arguments";
  case EDH_SERV_HND_M1_ERR_PAYLOAD_TOO_LARGE:
    return "payload exceeds maximum buffer size";
  case EDH_SERV_HND_M1_ERR_PREFIX_MISSING:
    return "missing CBOR true prefix";
  case EDH_SERV_HND_M1_ERR_CALLOC_FAILED:
    return "memory allocation failed";
  case EDH_SERV_HND_M1_ERR_COAP_SESSION_ALREADY_HAS_DATA:
    return "session already has EDHOC data";
  case EDH_SERV_HND_M1_ERR_EDHOC_CONTEXT_SETUP_FAILED:
    return "libedhoc context setup failed";
  case EDH_SERV_HND_M1_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED:
    return "libedhoc message_1_process failed";
  case EDH_SERV_HND_M1_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED:
    return "libedhoc message_2_compose failed";
  case EDH_SERV_HND_M1_OK:
    return "THIS SHOULD NEVER HAPPEN: failure logger called on success status";
  default:
    return "unknown error";
  }
}

coap_pdu_code_t cp_srv_m1_map_process_result(
    const edh_srv_hnd_m1_result_t message_1_result, coap_session_t* session) {
  if (message_1_result.status != EDH_SERV_HND_M1_OK) {
    const char* error_message =
        message_1_status_to_error_string(message_1_result.status);
    coap_log_err("Message 1 processing failed: %s\n", error_message);
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
