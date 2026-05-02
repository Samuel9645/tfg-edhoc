/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief CoAP adapter implementation that maps Message 1 EDHOC result codes
 * into transport responses.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/server/edhoc_message_process/srv_coap_m1_process.h"

#include <stdlib.h>

#include "edhoc/server/handshake/message_1/srv_m1_process.h"
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

static void cleanup_context(void* context) {
  if (context != NULL) {
    srv_edhoc_cleanup_context((struct edhoc_context**)&context);
  }
}

coap_pdu_code_t srv_coap_process_message_1_result(
    struct srv_edhoc_message_1_responder_result message_1_result,
    coap_session_t* session) {
  if (message_1_result.status != SRV_EDHOC_MSG1_RESPONDER_OK) {
    const char* error_message =
        srv_edhoc_message_1_responder_status_code_to_string(
            message_1_result.status);
    coap_log_err("Message 1 processing failed: %s\n", error_message);
    return map_message_1_status_to_response(message_1_result.status);
  }
  if (coap_session_set_app_data2(session, message_1_result.edhoc_ctx,
                                 cleanup_context) != NULL) {
    coap_log_err("already existing EDHOC context in CoAP session app data\n");
    srv_edhoc_cleanup_context(&message_1_result.edhoc_ctx);
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
  return map_message_1_status_to_response(message_1_result.status);
}
