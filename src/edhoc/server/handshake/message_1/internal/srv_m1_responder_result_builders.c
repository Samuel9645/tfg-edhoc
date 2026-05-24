/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of handling message 1 EDHOC requests
 */

#include "edhoc/server/handshake/message_1/internal/srv_m1_responder_result_builders.h"

#include <stddef.h>

struct srv_edhoc_message_1_responder_result srv_edhoc_message_1_responder_ok(
    const struct com_readonly_buffer message_2) {
  return (struct srv_edhoc_message_1_responder_result){
      .status = SRV_EDHOC_MSG1_RESPONDER_OK,
      .response = message_2};
}

struct srv_edhoc_message_1_responder_result
srv_edhoc_message_1_responder_failure(
    const enum srv_edhoc_message_1_responder_status status,
    const struct com_readonly_buffer error_message) {
  return (struct srv_edhoc_message_1_responder_result){
      .status = status, .response = error_message};
}

struct srv_edhoc_message_1_responder_result
srv_edhoc_message_1_responder_invalid_response_buffer_failure(void) {
  return (struct srv_edhoc_message_1_responder_result){
      .status = SRV_EDHOC_MSG1_RESPONDER_ERR_INVALID_RESPONSE_BUFFER};
}