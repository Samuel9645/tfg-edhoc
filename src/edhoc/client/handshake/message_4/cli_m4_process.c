/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Processing implementation for client handshake Message 4.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_4/cli_m4_process.h"

#include "edhoc/client/handshake/message_4/cli_m4_errors.h"

static struct edh_cli_message_4_process_result message_4_process_ok(
    const struct com_writable_buffer message_4) {
  return (struct edh_cli_message_4_process_result){
      .status = EDH_CLI_MSG4_PROCESS_OK,
      .output = message_4,
  };
}

static struct edh_cli_message_4_process_result
message_4_process_protocol_failure(
    const enum edh_cli_message_4_process_status status,
    const struct com_writable_buffer error_buffer) {
  return (struct edh_cli_message_4_process_result){
      .status = status,
      .output = error_buffer,
  };
}

static struct edh_cli_message_4_process_result message_4_process_local_failure(
    const enum edh_cli_message_4_process_status status) {
  return (struct edh_cli_message_4_process_result){
      .status = status,
  };
}

struct edh_cli_message_4_process_result edh_cli_process_message_4(
    struct edh_cli_handshake* state, const struct com_readonly_buffer message_4,
    struct com_writable_buffer* message_4_error) {
  if (!edh_cli_handshake_is_initialized(state) ||
      !com_readonly_buffer_is_valid(message_4) ||
      !com_writable_buffer_is_writable(message_4_error)) {
    return message_4_process_local_failure(
        EDH_CLI_MSG4_PROCESS_ERR_INVALID_ARGS);
  }

  const int edhoc_result = edhoc_message_4_process(
      &state->context, message_4.bytes, message_4.length);
  if (edhoc_result != EDHOC_SUCCESS) {
    edh_cli_message_4_handler_add_error(
        edhoc_result, "Failed to process EDHOC message 4", message_4_error);
    return message_4_process_protocol_failure(
        EDH_CLI_MSG4_PROCESS_ERR_EDHOC_MESSAGE_4_PROCESS_FAILED,
        *message_4_error);
  }

  message_4_error->length = 0;
  return message_4_process_ok(*message_4_error);
}
