/**
 * @file process.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Processing implementation for client handshake Message 2.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_2/process.h"

#include "edhoc/client/handshake/message_2/errors.h"

static edh_cli_message_2_result_t edh_cli_message_2_create_result(
    const edh_cli_message_2_process_status_t status,
    const com_writable_buffer_t* message_2_error) {
  edh_cli_message_2_result_t result = {
      .status = status,
      .output = {0},
  };
  if (message_2_error != NULL) {
    result.output = *message_2_error;
  }
  return result;
}

edh_cli_message_2_result_t edh_cli_process_message_2(
    edh_cli_handshake_t* state, const com_readonly_buffer_t* message_2,
    com_writable_buffer_t* message_2_error) {
  if (!edh_cli_handshake_is_initialized(state) ||
      !com_readonly_buffer_is_valid(message_2) ||
      !com_writable_buffer_is_writable(message_2_error)) {
    return edh_cli_message_2_create_result(
        EDH_CLI_MSG2_PROCESS_ERR_INVALID_ARGS, message_2_error);
  }

  const int edhoc_result = edhoc_message_2_process(
      &state->context, message_2->bytes, message_2->length);
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
