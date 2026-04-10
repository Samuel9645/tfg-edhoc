/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/10 Composition implementation for client handshake
 * Message 1.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_1/cli_m1_compose.h"

#include <edhoc.h>
#include <edhoc_helpers.h>

// TODO: Write errors into response buffer.

static struct cli_edhoc_message_1_compose_result message_1_compose_ok(
    const struct com_writable_buffer message_1) {
  return (struct cli_edhoc_message_1_compose_result){
      .status = CLI_EDHOC_MSG1_COMPOSE_OK, .output = message_1};
}

static struct cli_edhoc_message_1_compose_result
message_1_compose_protocol_failure(
    const enum cli_edhoc_message_1_compose_status status,
    const struct com_writable_buffer error_buffer) {
  return (struct cli_edhoc_message_1_compose_result){.status = status,
                                                     .output = error_buffer};
}

static struct cli_edhoc_message_1_compose_result
message_1_compose_local_failure(
    const enum cli_edhoc_message_1_compose_status status) {
  return (struct cli_edhoc_message_1_compose_result){.status = status};
}

struct cli_edhoc_message_1_compose_result cli_edhoc_compose_message_1(
    struct cli_edhoc_handshake* state, struct com_writable_buffer* message_1) {
  if (!cli_edhoc_handshake_is_initialized(state) ||
      !com_writable_buffer_is_writable(message_1)) {
    return message_1_compose_local_failure(
        CLI_EDHOC_MSG1_COMPOSE_ERR_INVALID_ARGS);
  }

  struct edhoc_prepended_fields prepended_fields = {
      .buffer = message_1->bytes,
      .buffer_size = message_1->capacity,
      .edhoc_message_ptr = message_1->bytes,
      .edhoc_message_size = message_1->capacity};
  if (edhoc_prepend_flow(&prepended_fields) != EDHOC_SUCCESS) {
    return message_1_compose_protocol_failure(
        CLI_EDHOC_MSG1_COMPOSE_ERR_EDHOC_PREPEND_FAILED, *message_1);
  }
  if (edhoc_message_1_compose(
          &state->context, prepended_fields.edhoc_message_ptr,
          prepended_fields.edhoc_message_size,
          &prepended_fields.edhoc_message_size) != EDHOC_SUCCESS) {
    return message_1_compose_protocol_failure(
        CLI_EDHOC_MSG1_COMPOSE_ERR_EDHOC_MESSAGE_1_COMPOSE_FAILED, *message_1);
  }
  if (edhoc_prepend_recalculate_size(&prepended_fields) != EDHOC_SUCCESS) {
    return message_1_compose_protocol_failure(
        EDHOC_MSG1_COMPOSE_ERR_EDHOC_PREPEND_SIZE_CALC_FAILED, *message_1);
  }
  message_1->length = prepended_fields.buffer_size;
  return message_1_compose_ok(*message_1);
}
