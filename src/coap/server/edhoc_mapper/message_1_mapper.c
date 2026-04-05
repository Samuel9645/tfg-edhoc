/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief CoAP adapter implementation that maps Message 1 EDHOC result codes
 * into transport responses.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/server/edhoc_mapper/message_1_mapper.h"

#include <stdlib.h>

#include "edhoc/server/handshake/message_1/srv_m1_handler.h"

static coap_pdu_code_t map_message_1_status_to_response(
    const enum edh_srv_message_1_handler_status status) {
  switch (status) {
  case EDH_SRV_MSG1_HDL_OK:
    return COAP_RESPONSE_CODE_CHANGED;

  case EDH_SRV_MSG1_HDL_ERR_INVALID_REQUEST_BUFFER:
  case EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED:
    return COAP_RESPONSE_CODE_BAD_REQUEST;

  case EDH_SRV_MSG1_HDL_ERR_NULL_CREDENTIALS:
  case EDH_SRV_MSG1_HDL_ERR_INVALID_RESPONSE_BUFFER:
  case EDH_SRV_MSG1_HDL_ERR_CALLOC_FAILED:
  case EDH_SRV_MSG1_HDL_ERR_EDHOC_CONTEXT_SETUP_FAILED:
  case EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED:
  default:
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
}

coap_pdu_code_t cp_srv_map_message_1_result_to_coap(
    const ehd_srv_message_1_handler_result_t message_1_result,
    coap_session_t* session) {
  if (message_1_result.status != EDH_SRV_MSG1_HDL_OK) {
    const char* error_message =
        edh_srv_handle_message_1_status_code_to_string(message_1_result.status);
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
