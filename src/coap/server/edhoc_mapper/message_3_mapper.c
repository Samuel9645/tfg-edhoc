/**
 * @file message_3_mapper.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief CoAP adapter implementation that maps Message 3 EDHOC result codes
 * into transport responses.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/server/edhoc_mapper/message_3_mapper.h"

static coap_pdu_code_t map_message_3_status_to_response(
    const edh_srv_hnd_m3_result_t result) {
  switch (result) {
  case EDH_SRV_HND_M3_OK:
    return COAP_RESPONSE_CODE_CHANGED;

  case EDH_SRV_HND_M3_ERR_INVALID_ARGS:
  case EDH_SRV_HND_M3_ERR_MESSAGE_3_PROCESS_FAILED:
    return COAP_RESPONSE_CODE_BAD_REQUEST;

  case EDH_SRV_HND_M3_ERR_MESSAGE_4_COMPOSE_FAILED:
  default:
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
}

static const char* message_3_result_to_error_string(
    const edh_srv_hnd_m3_result_t result) {
  switch (result) {
  case EDH_SRV_HND_M3_ERR_INVALID_ARGS:
    return "invalid arguments or missing context";
  case EDH_SRV_HND_M3_ERR_MESSAGE_3_PROCESS_FAILED:
    return "libedhoc message_3_process failed";
  case EDH_SRV_HND_M3_ERR_MESSAGE_4_COMPOSE_FAILED:
    return "libedhoc message_4_compose failed";
  case EDH_SRV_HND_M3_OK:
    return "THIS SHOULD NEVER HAPPEN: failure logger called on success status";
  default:
    return "unknown error";
  }
}

coap_pdu_code_t cp_srv_m3_map_process_result(
    const edh_srv_hnd_m3_result_t message_3_result) {
  if (message_3_result != EDH_SRV_HND_M3_OK) {
    const char* error_message =
        message_3_result_to_error_string(message_3_result);
    coap_log_err("Message 3 processing failed: %s\n", error_message);
  }
  return map_message_3_status_to_response(message_3_result);
}
