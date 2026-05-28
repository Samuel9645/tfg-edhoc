/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/10
 * @brief Composition implementation for client handshake Message 1.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_1/cli_m1_compose.h"

#include <edhoc.h>
#include <edhoc_helpers.h>

#include "common/com_logging.h"

static struct cli_edhoc_message_1_compose_result ok(
    const struct com_readonly_buffer message_1) {
  return (struct cli_edhoc_message_1_compose_result){
      .status = CLI_EDHOC_MSG1_COMPOSE_OK, .buffer = message_1};
}

static struct cli_edhoc_message_1_compose_result protocol_failure(
    const enum cli_edhoc_message_1_compose_status status) {
  return (struct cli_edhoc_message_1_compose_result){.status = status};
}

static struct cli_edhoc_message_1_compose_result invalid_compose_buffer(void) {
  return (struct cli_edhoc_message_1_compose_result){
      .status = CLI_EDHOC_MSG1_COMPOSE_ERR_INVALID_COMPOSE_BUFFER};
}

struct cli_edhoc_message_1_compose_result cli_edhoc_compose_message_1(
    struct edhoc_context* context,
    const struct com_writable_buffer compose_buffer) {
  if (!com_writable_buffer_is_writable(compose_buffer)) {
    return invalid_compose_buffer();
  }

  size_t message_1_length = compose_buffer.capacity;
  if (edhoc_message_1_compose(context, compose_buffer.bytes,
                              compose_buffer.capacity,
                              &message_1_length) != EDHOC_SUCCESS) {
    com_log_error("Message 1 Compose error: Failed to compose EDHOC message");
    return protocol_failure(CLI_EDHOC_MSG1_COMPOSE_ERR_EDHOC_MESSAGE_1_COMPOSE);
  }
  const struct com_readonly_conversion_result conversion_result =
      com_writable_as_readonly(compose_buffer, message_1_length);
  if (conversion_result.status != COM_RDONLY_CONV_OK) {
    com_log_error("Message 1 Compose error: Empty compose result");
    return protocol_failure(CLI_EDHOC_MSG1_COMPOSE_ERR_EMPTY_COMPOSE);
  }
  return ok(conversion_result.buffer);
}
