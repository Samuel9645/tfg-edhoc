/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 16/04/2026
 * @brief Definitions for the srv_m1_process_result_builders module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/internal/srv_m1_process_result_builders.h"

struct srv_edhoc_message_1_process_result srv_edhoc_message_1_process_failure(
    const enum srv_edhoc_message_1_process_status status,
    const struct com_readonly_buffer error_buffer) {
  return (struct srv_edhoc_message_1_process_result){
      .status = status, .error_buffer = error_buffer};
}

struct srv_edhoc_message_1_process_result srv_edhoc_message_1_process_ok(
    struct edhoc_context* context) {
  return (struct srv_edhoc_message_1_process_result){
      .status = SRV_EDHOC_MSG1_PROCESS_OK, .context = context};
}

struct srv_edhoc_message_1_process_result
srv_edhoc_message_1_process_invalid_error_buffer_failure(void) {
  return (struct srv_edhoc_message_1_process_result){
      .status = SRV_EDHOC_MSG1_PROCESS_ERR_INVALID_ERROR_BUFFER};
}