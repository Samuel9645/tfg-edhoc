/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Processing implementation for client handshake Message 2.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_2/cli_m2_process.h"

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

static struct cli_edhoc_message_2_process_result ok(void) {
  return (struct cli_edhoc_message_2_process_result){
      .status = CLI_EDHOC_MSG2_PROCESS_OK,
  };
}

static struct cli_edhoc_message_2_process_result process_protocol_failure(
    const struct com_readonly_buffer error_buffer) {
  return (struct cli_edhoc_message_2_process_result){
      .status = CLI_EDHOC_MSG2_PROCESS_ERR_EDHOC_MESSAGE_2_PROCESS_FAILED,
      .error_buffer = error_buffer,
  };
}

static struct cli_edhoc_message_2_process_result invalid_error_buffer(void) {
  return (struct cli_edhoc_message_2_process_result){
      .status = CLI_EDHOC_MSG2_PROCESS_ERR_INVALID_ERROR_BUFFER};
}

static struct cli_edhoc_message_2_process_result internal_failure(
    const enum cli_edhoc_message_2_process_status status,
    const struct com_readonly_buffer error_buffer) {
  return (struct cli_edhoc_message_2_process_result){
      .status = status, .error_buffer = error_buffer};
}

struct cli_edhoc_message_2_process_result cli_edhoc_process_message_2(
    struct edhoc_context* context, const struct com_readonly_buffer message_2,
    const struct com_writable_buffer error_buffer) {
  if (!com_writable_buffer_is_writable(error_buffer)) {
    return invalid_error_buffer();
  }
  if (context == NULL) {
    return internal_failure(
        CLI_EDHOC_MSG2_PROCESS_ERR_NULL_CONTEXT,
        com_edhoc_add_internal_error_view("Null EDHOC context", error_buffer));
  }
  if (!com_readonly_buffer_has_content(message_2)) {
    return internal_failure(
        CLI_EDHOC_MSG2_PROCESS_ERR_EMPTY_MESSAGE_2,
        com_edhoc_add_internal_error_view("Empty Message 2", error_buffer));
  }

  if (edhoc_message_2_process(context, message_2.bytes, message_2.length) !=
      EDHOC_SUCCESS) {
    return process_protocol_failure(
        com_edhoc_add_protocol_error_with_description_view(
            context, "Failed to process EDHOC message 2", error_buffer));
  }
  return ok();
}