/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Definitions for the srv_m3_process_result_builders module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_3/internal/srv_m3_process_result_builders.h"

struct srv_edhoc_message_3_process_result
srv_edhoc_message_3_process_invalid_error_buffer_failure(void) {
  return (struct srv_edhoc_message_3_process_result){
      .status = SRV_EDHOC_MSG3_PROCESS_ERR_INVALID_ERROR_BUFFER};
}

struct srv_edhoc_message_3_process_result srv_edhoc_message_3_process_failure(
    const enum srv_edhoc_message_3_process_status status,
    const struct com_readonly_buffer error_buffer) {
  return (struct srv_edhoc_message_3_process_result){
      .status = status, .error_buffer = error_buffer};
}

struct srv_edhoc_message_3_process_result srv_edhoc_message_3_process_ok(void) {
  return (struct srv_edhoc_message_3_process_result){
      .status = SRV_EDHOC_MSG3_PROCESS_OK};
}
