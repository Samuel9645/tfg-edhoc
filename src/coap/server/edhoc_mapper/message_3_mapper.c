/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief CoAP adapter implementation that maps Message 3 EDHOC result codes
 * into transport responses.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/server/edhoc_mapper/message_3_mapper.h"

#include "edhoc/server/handshake/message_3/srv_m3_handler.h"

static coap_pdu_code_t map_message_3_status_to_response(
    const enum edh_srv_message_3_handler_status result) {
  switch (result) {
  case EDH_MSG3_HDL_OK:
    return COAP_RESPONSE_CODE_CHANGED;

  case EDH_MSG3_HDL_ERR_MESSAGE_3_PROCESS_FAILED:
    return COAP_RESPONSE_CODE_BAD_REQUEST;

  case EDH_MSG3_HDL_ERR_INVALID_PARSED_MESSAGE_3:
  case EDH_MSG3_HDL_ERR_INVALID_RESPONSE_BUFFER:
  case EDH_MSG3_HDL_ERR_NULL_EDHOC_CONTEXT:
  case EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_FAILED:
  default:
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
}

coap_pdu_code_t cp_srv_map_message_3_result_to_coap(
    const enum edh_srv_message_3_handler_status message_3_result) {
  if (message_3_result != EDH_MSG3_HDL_OK) {
    const char* error_message =
        edh_srv_handle_message_3_status_code_to_string(message_3_result);
    coap_log_err("Message 3 processing failed: %s\n", error_message);
  }
  return map_message_3_status_to_response(message_3_result);
}
