/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Processing implementation for client handshake Message 4.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_4/cli_m4_process.h"

#include "../../../../../include/edhoc/common/add_error/edhoc_error.h"

static struct cli_edhoc_message_4_process_result message_4_process_ok(
    const struct com_writable_buffer message_4) {
  return (struct cli_edhoc_message_4_process_result){
      .status = CLI_EDHOC_MSG4_PROCESS_OK,
      .output = message_4,
  };
}

static struct cli_edhoc_message_4_process_result
message_4_process_protocol_failure(
    const enum cli_edhoc_message_4_process_status status,
    const struct com_writable_buffer error_buffer) {
  return (struct cli_edhoc_message_4_process_result){
      .status = status,
      .output = error_buffer,
  };
}

static struct cli_edhoc_message_4_process_result
message_4_process_local_failure(
    const enum cli_edhoc_message_4_process_status status) {
  return (struct cli_edhoc_message_4_process_result){
      .status = status,
  };
}

struct cli_edhoc_message_4_process_result cli_edhoc_process_message_4(
    struct cli_edhoc_handshake* state,
    const struct com_readonly_buffer message_4,
    struct com_writable_buffer* message_4_error) {
  if (!cli_edhoc_handshake_is_initialized(state) ||
      !com_readonly_buffer_is_valid(message_4) ||
      !com_writable_buffer_is_writable(message_4_error)) {
    return message_4_process_local_failure(
        CLI_EDHOC_MSG4_PROCESS_ERR_INVALID_ARGS);
  }

  const int edhoc_result = edhoc_message_4_process(
      &state->context, message_4.bytes, message_4.length);
  if (edhoc_result != EDHOC_SUCCESS) {
    (void)com_edhoc_add_edhoc_error_to_response_with_description(
        &state->context, "Failed to process EDHOC message 4", message_4_error);
    return message_4_process_protocol_failure(
        CLI_EDHOC_MSG4_PROCESS_ERR_EDHOC_MESSAGE_4_PROCESS_FAILED,
        *message_4_error);
  }

  message_4_error->length = 0;
  return message_4_process_ok(*message_4_error);
}
