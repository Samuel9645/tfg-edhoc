/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief CoAP adapter implementation that maps Message 1 EDHOC result codes
 * into transport responses.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/server/edhoc_message_process/srv_coap_m1_process.h"

#include "edhoc/server/handshake/message_1/srv_m1_responder.h"

static coap_pdu_code_t map_message_1_status_to_response(
    const enum srv_edhoc_message_1_responder_status status) {
  switch (status) {
  case SRV_EDHOC_MSG1_RESPONDER_OK:
    return COAP_RESPONSE_CODE_CHANGED;

  case SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_1_PROCESS_FAILED:
    return COAP_RESPONSE_CODE_BAD_REQUEST;

  case SRV_EDHOC_MSG1_RESPONDER_ERR_INVALID_RESPONSE_BUFFER:
  case SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_2_COMPOSE_FAILED:
  default:
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
}

coap_pdu_code_t srv_coap_process_message_1_result(
    const struct srv_edhoc_message_1_responder_result message_1_result) {
  if (message_1_result.status != SRV_EDHOC_MSG1_RESPONDER_OK) {
    const char* error_message =
        srv_edhoc_message_1_responder_status_code_to_string(
            message_1_result.status);
    coap_log_err("Message 1 responder failed: %s\n", error_message);
  }
  return map_message_1_status_to_response(message_1_result.status);
}
