/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Processing implementation for client handshake Message 2.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_2/cli_m2_process.h"

#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

static struct cli_edhoc_message_2_process_result message_2_process_ok(void) {
  return (struct cli_edhoc_message_2_process_result){
      .status = CLI_EDHOC_MSG2_PROCESS_OK,
  };
}

static struct cli_edhoc_message_2_process_result
message_2_process_protocol_failure(
    const enum cli_edhoc_message_2_process_status status,
    const struct com_writable_buffer error_buffer) {
  return (struct cli_edhoc_message_2_process_result){
      .status = status,
      .output = error_buffer,
  };
}

static struct cli_edhoc_message_2_process_result
message_2_process_local_failure(
    const enum cli_edhoc_message_2_process_status status) {
  return (struct cli_edhoc_message_2_process_result){
      .status = status,
  };
}

struct cli_edhoc_message_2_process_result cli_edhoc_process_message_2(
    struct cli_edhoc_handshake* state,
    const struct com_readonly_buffer message_2,
    struct com_writable_buffer* message_2_error) {
  if (!cli_edhoc_handshake_is_initialized(state) ||
      !com_readonly_buffer_is_valid(message_2) ||
      !com_writable_buffer_is_writable(message_2_error)) {
    return message_2_process_local_failure(
        CLI_EDHOC_MSG2_PROCESS_ERR_INVALID_ARGS);
  }

  const int edhoc_result = edhoc_message_2_process(
      &state->context, message_2.bytes, message_2.length);

  if (edhoc_result != EDHOC_SUCCESS) {
    com_edhoc_add_edhoc_error_to_response_with_description(
        &state->context, "Failed to process EDHOC message 2", message_2_error);
    return message_2_process_protocol_failure(
        CLI_EDHOC_MSG2_PROCESS_ERR_EDHOC_MESSAGE_2_PROCESS_FAILED,
        *message_2_error);
  }
  message_2_error->length = 0;
  return message_2_process_ok();
}