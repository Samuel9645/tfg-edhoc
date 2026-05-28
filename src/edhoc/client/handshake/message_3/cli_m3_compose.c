/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Message 3 Composition for the client
 * @see [RFC
 * 9528 5.4.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-initiator-composition-of-mes)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_3/cli_m3_compose.h"

#include <edhoc_helpers.h>

#include "common/com_logging.h"

static struct cli_edhoc_message_3_compose_result ok(
    const struct com_readonly_buffer message_3) {
  return (struct cli_edhoc_message_3_compose_result){
      .status = CLI_EDHOC_MSG3_COMPOSE_OK,
      .buffer = message_3,
  };
}

static struct cli_edhoc_message_3_compose_result protocol_failure(
    const enum cli_edhoc_message_3_compose_status status) {
  return (struct cli_edhoc_message_3_compose_result){.status = status};
}

static struct cli_edhoc_message_3_compose_result invalid_compose_buffer(void) {
  return (struct cli_edhoc_message_3_compose_result){
      .status = CLI_EDHOC_MSG3_COMPOSE_ERR_INVALID_COMPOSE_BUFFER,
  };
}

static struct cli_edhoc_message_3_compose_result null_context_failure(void) {
  return (struct cli_edhoc_message_3_compose_result){
      .status = CLI_EDHOC_MSG3_COMPOSE_ERR_NULL_CONTEXT};
}

struct cli_edhoc_message_3_compose_result cli_edhoc_compose_message_3(
    struct edhoc_context* context,
    const struct com_writable_buffer compose_buffer) {
  if (!com_writable_buffer_is_writable(compose_buffer)) {
    return invalid_compose_buffer();
  }
  if (context == NULL) {
    com_log_error("Message 3 Compose error: Null context");
    return null_context_failure();
  }

  size_t message_3_length = compose_buffer.capacity;
  if (edhoc_message_3_compose(context, compose_buffer.bytes,
                              compose_buffer.capacity,
                              &message_3_length) != EDHOC_SUCCESS) {
    com_log_error("Message 3 Compose error: Failed to compose EDHOC message");
    return protocol_failure(
        CLI_EDHOC_MSG3_COMPOSE_ERR_EDHOC_MESSAGE_3_COMPOSE_FAILED);
  }
  const struct com_readonly_conversion_result conversion_result =
      com_writable_as_readonly(compose_buffer, message_3_length);
  if (conversion_result.status != COM_RDONLY_CONV_OK) {
    com_log_error("Message 3 Compose error: Empty compose result");
    return protocol_failure(CLI_EDHOC_MSG3_COMPOSE_ERR_EMPTY_COMPOSE);
  }
  return ok(conversion_result.buffer);
}
