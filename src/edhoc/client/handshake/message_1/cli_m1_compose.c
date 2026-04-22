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

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

static struct cli_edhoc_message_1_compose_result ok(
    const struct com_readonly_buffer message_1) {
  return (struct cli_edhoc_message_1_compose_result){
      .status = CLI_EDHOC_MSG1_COMPOSE_OK, .buffer = message_1};
}

static struct cli_edhoc_message_1_compose_result protocol_failure(
    const enum cli_edhoc_message_1_compose_status status,
    const struct com_readonly_buffer buffer) {
  return (struct cli_edhoc_message_1_compose_result){
      .status = status,
      .buffer = buffer,
  };
}

static struct cli_edhoc_message_1_compose_result invalid_compose_buffer(void) {
  return (struct cli_edhoc_message_1_compose_result){
      .status = CLI_EDHOC_MSG1_COMPOSE_ERR_INVALID_COMPOSE_BUFFER};
}

struct cli_edhoc_message_1_compose_result cli_edhoc_compose_message_1(
    struct edhoc_context* context, struct com_writable_buffer* compose_buffer) {
  if (!com_writable_buffer_is_writable(compose_buffer)) {
    return invalid_compose_buffer();
  }

  struct edhoc_prepended_fields prepended_fields = {
      .buffer = compose_buffer->bytes,
      .buffer_size = compose_buffer->capacity,
      .edhoc_message_ptr = compose_buffer->bytes,
      .edhoc_message_size = compose_buffer->capacity};
  if (edhoc_prepend_flow(&prepended_fields) != EDHOC_SUCCESS) {
    return protocol_failure(
        CLI_EDHOC_MSG1_COMPOSE_ERR_EDHOC_PREPEND,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Failed to prepend flow for message 1", compose_buffer));
  }
  if (edhoc_message_1_compose(context, prepended_fields.edhoc_message_ptr,
                              prepended_fields.edhoc_message_size,
                              &prepended_fields.edhoc_message_size) !=
      EDHOC_SUCCESS) {
    return protocol_failure(
        CLI_EDHOC_MSG1_COMPOSE_ERR_EDHOC_MESSAGE_1_COMPOSE,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Failed to compose EDHOC message 1", compose_buffer));
  }
  if (edhoc_prepend_recalculate_size(&prepended_fields) != EDHOC_SUCCESS) {
    return protocol_failure(
        CLI_EDHOC_MSG1_COMPOSE_ERR_EDHOC_PREPEND_SIZE_CALC,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Failed to recalculate prepended size for message 1",
            compose_buffer));
  }
  compose_buffer->length = prepended_fields.buffer_size;
  const struct com_readonly_conversion_result conversion_result =
      com_writable_as_readonly(compose_buffer);
  if (conversion_result.status != COM_RDONLY_CONV_OK) {
    // SHOULD NEVER HAPPEN
    return invalid_compose_buffer();
  }
  return ok(conversion_result.buffer);
}
