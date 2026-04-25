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

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

static struct cli_edhoc_message_3_compose_result ok(
    const struct com_readonly_buffer message_3) {
  return (struct cli_edhoc_message_3_compose_result){
      .status = CLI_EDHOC_MSG3_COMPOSE_OK,
      .buffer = message_3,
  };
}

static struct cli_edhoc_message_3_compose_result protocol_failure(
    const enum cli_edhoc_message_3_compose_status status,
    const struct com_readonly_buffer error_buffer) {
  return (struct cli_edhoc_message_3_compose_result){
      .status = status,
      .buffer = error_buffer,
  };
}

static struct cli_edhoc_message_3_compose_result empty_compose_failure(
    const struct com_readonly_buffer error_buffer) {
  return (struct cli_edhoc_message_3_compose_result){
      .status = CLI_EDHOC_MSG3_COMPOSE_ERR_EMPTY_COMPOSE,
      .buffer = error_buffer};
}

static struct cli_edhoc_message_3_compose_result invalid_compose_buffer(void) {
  return (struct cli_edhoc_message_3_compose_result){
      .status = CLI_EDHOC_MSG3_COMPOSE_ERR_INVALID_COMPOSE_BUFFER,
  };
}

static struct cli_edhoc_message_3_compose_result null_context_failure(
    const struct com_writable_buffer compose_buffer) {
  return (struct cli_edhoc_message_3_compose_result){
      .status = CLI_EDHOC_MSG3_COMPOSE_ERR_NULL_CONTEXT,
      .buffer = com_edhoc_add_internal_error_view(
          "Context pointer is NULL when composing message 3", compose_buffer),
  };
}

struct cli_edhoc_message_3_compose_result cli_edhoc_compose_message_3(
    struct edhoc_context* context,
    const struct com_writable_buffer compose_buffer) {
  if (!com_writable_buffer_is_writable(compose_buffer)) {
    return invalid_compose_buffer();
  }
  if (context == NULL) {
    return null_context_failure(compose_buffer);
  }

  struct edhoc_prepended_fields prepended_fields = {
      .buffer = compose_buffer.bytes,
      .buffer_size = compose_buffer.capacity,
      .edhoc_message_ptr = compose_buffer.bytes,
      .edhoc_message_size = compose_buffer.capacity,
  };

  if (edhoc_prepend_connection_id(
          &prepended_fields, &context->private_peer_cid) != EDHOC_SUCCESS) {
    return protocol_failure(
        CLI_EDHOC_MSG3_COMPOSE_ERR_CONNECTION_ID_PREPEND_FAILED,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Failed to prepend connection id for message 3",
            compose_buffer));
  }

  if (edhoc_message_3_compose(context, prepended_fields.edhoc_message_ptr,
                              prepended_fields.edhoc_message_size,
                              &prepended_fields.edhoc_message_size) !=
      EDHOC_SUCCESS) {
    return protocol_failure(
        CLI_EDHOC_MSG3_COMPOSE_ERR_EDHOC_MESSAGE_3_COMPOSE_FAILED,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Failed to compose EDHOC message 3", compose_buffer));
  }
  if (edhoc_prepend_recalculate_size(&prepended_fields) != EDHOC_SUCCESS) {
    return protocol_failure(
        CLI_EDHOC_MSG3_COMPOSE_ERR_PREPEND_RECALCULATION_FAILED,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Failed to recalculate prepended message size",
            compose_buffer));
  }
  const struct com_readonly_conversion_result conversion_result =
      com_writable_as_readonly(compose_buffer, prepended_fields.buffer_size);
  if (conversion_result.status != COM_RDONLY_CONV_OK) {
    return empty_compose_failure(com_edhoc_add_internal_error_view(
        "Empty compose result", compose_buffer));
  }
  return ok(conversion_result.buffer);
}
