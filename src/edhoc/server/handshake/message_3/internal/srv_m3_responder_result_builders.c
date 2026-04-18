/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Definitions for the srv_m3_responder_result_builders module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_3/internal/srv_m3_responder_result_builders.h"

#include <stddef.h>

struct srv_edhoc_message_3_responder_result
srv_edhoc_message_3_responder_invalid_response_buffer_failure(void) {
  return (struct srv_edhoc_message_3_responder_result){
      .status = SRV_EDHOC_MSG3_RESPONDER_ERR_INVALID_RESPONSE_BUFFER};
}

struct srv_edhoc_message_3_responder_result
srv_edhoc_message_3_responder_failure(
    const enum srv_edhoc_message_3_responder_status status) {
  return (struct srv_edhoc_message_3_responder_result){.status = status};
}

struct srv_edhoc_message_3_responder_result srv_edhoc_message_3_responder_ok() {
  return (struct srv_edhoc_message_3_responder_result){
      .status = SRV_EDHOC_MSG3_RESPONDER_OK};
}
