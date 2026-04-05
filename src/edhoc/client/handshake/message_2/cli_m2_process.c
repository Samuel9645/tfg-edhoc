/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Processing implementation for client handshake Message 2.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_2/cli_m2_process.h"

#include "edhoc/client/handshake/message_2/cli_m2_errors.h"

static struct edh_cli_message_2_result edh_cli_message_2_create_result(
    const enum edh_cli_message_2_process_status status,
    const struct com_writable_buffer* message_2_error) {
  struct edh_cli_message_2_result result = {
      .status = status,
      .output = {0},
  };
  if (message_2_error != NULL) {
    result.output = *message_2_error;
  }
  return result;
}

struct edh_cli_message_2_result edh_cli_process_message_2(
    struct edh_cli_handshake* state, const struct com_readonly_buffer message_2,
    struct com_writable_buffer* message_2_error) {
  if (!edh_cli_handshake_is_initialized(state) ||
      !com_readonly_buffer_is_valid(message_2) ||
      !com_writable_buffer_is_writable(message_2_error)) {
    return edh_cli_message_2_create_result(
        EDH_CLI_MSG2_PROCESS_ERR_INVALID_ARGS, message_2_error);
  }

  const int edhoc_result = edhoc_message_2_process(
      &state->context, message_2.bytes, message_2.length);
  if (edhoc_result != EDHOC_SUCCESS) {
    edh_cli_message_2_handler_add_error(
        edhoc_result, "Failed to process EDHOC message 2", message_2_error);
    return edh_cli_message_2_create_result(
        EDH_CLI_MSG2_PROCESS_ERR_EDHOC_MESSAGE_2_PROCESS_FAILED,
        message_2_error);
  }

  message_2_error->length = 0;
  return edh_cli_message_2_create_result(EDH_CLI_MSG2_PROCESS_OK,
                                         message_2_error);
}
