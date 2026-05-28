/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Processing implementation for client handshake Message 4.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_4/cli_m4_process.h"

#include "common/com_logging.h"

static struct cli_edhoc_message_4_process_result ok(void) {
  return (struct cli_edhoc_message_4_process_result){
      .status = CLI_EDHOC_MSG4_PROCESS_OK,
  };
}

static struct cli_edhoc_message_4_process_result internal_failure(
    const enum cli_edhoc_message_4_process_status status) {
  return (struct cli_edhoc_message_4_process_result){.status = status};
}

struct cli_edhoc_message_4_process_result cli_edhoc_process_message_4(
    struct edhoc_context* context, const struct com_readonly_buffer message_4) {
  if (context == NULL) {
    com_log_error("Message 4 Process error: Null context");
    return internal_failure(CLI_EDHOC_MSG4_PROCESS_ERR_NULL_CONTEXT);
  }
  if (!com_readonly_buffer_has_content(message_4)) {
    com_log_error("Message 4 Process error: Empty message");
    return internal_failure(CLI_EDHOC_MSG4_PROCESS_ERR_EMPTY_MESSAGE_4);
  }

  if (edhoc_message_4_process(context, message_4.bytes, message_4.length) !=
      EDHOC_SUCCESS) {
    com_log_error("Message 4 Process error: Failed to process EDHOC message");
    return internal_failure(
        CLI_EDHOC_MSG4_PROCESS_ERR_EDHOC_MESSAGE_4_PROCESS_FAILED);
  }
  return ok();
}
