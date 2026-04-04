/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Processing implementation for client handshake Message 4.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_4/cli_m4_errors.h"
#include "edhoc/client/handshake/message_4/cli_m4_process.h"

static edh_cli_message_4_result_t edh_cli_message_4_create_result(
    const edh_cli_message_4_process_status_t status,
    const com_writable_buffer_t* message_4_error) {
  edh_cli_message_4_result_t result = {
      .status = status,
      .output = {0},
  };
  if (message_4_error != NULL) {
    result.output = *message_4_error;
  }
  return result;
}

edh_cli_message_4_result_t edh_cli_process_message_4(
    edh_cli_handshake_t* state, const com_readonly_buffer_t message_4,
    com_writable_buffer_t* message_4_error) {
  if (!edh_cli_handshake_is_initialized(state) ||
      !com_readonly_buffer_is_valid(message_4) ||
      !com_writable_buffer_is_writable(message_4_error)) {
    return edh_cli_message_4_create_result(
        EDH_CLI_MSG4_PROCESS_ERR_INVALID_ARGS, message_4_error);
  }

  const int edhoc_result = edhoc_message_4_process(
      &state->context, message_4.bytes, message_4.length);
  if (edhoc_result != EDHOC_SUCCESS) {
    edh_cli_message_4_handler_add_error(
        edhoc_result, "Failed to process EDHOC message 4", message_4_error);
    return edh_cli_message_4_create_result(
        EDH_CLI_MSG4_PROCESS_ERR_EDHOC_MESSAGE_4_PROCESS_FAILED,
        message_4_error);
  }

  message_4_error->length = 0;
  return edh_cli_message_4_create_result(EDH_CLI_MSG4_PROCESS_OK,
                                         message_4_error);
}
