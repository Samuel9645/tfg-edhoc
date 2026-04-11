/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Composition implementation for client handshake Message 3.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_3/cli_m3_compose.h"

#include <edhoc_helpers.h>

#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

static struct cli_edhoc_message_3_compose_result message_3_compose_ok(
    const struct com_writable_buffer message_3) {
  return (struct cli_edhoc_message_3_compose_result){
      .status = CLI_EDHOC_MSG3_COMPOSE_OK,
      .output = message_3,
  };
}

static struct cli_edhoc_message_3_compose_result
message_3_compose_protocol_failure(
    const enum cli_edhoc_message_3_compose_status status,
    const struct com_writable_buffer error_buffer) {
  return (struct cli_edhoc_message_3_compose_result){
      .status = status,
      .output = error_buffer,
  };
}

static struct cli_edhoc_message_3_compose_result
message_3_compose_local_failure(
    const enum cli_edhoc_message_3_compose_status status) {
  return (struct cli_edhoc_message_3_compose_result){
      .status = status,
  };
}

struct cli_edhoc_message_3_compose_result cli_edhoc_compose_message_3(
    struct cli_edhoc_handshake* state,
    struct com_writable_buffer* message_3_or_error) {
  const size_t min_payload_capacity = 1;
  if (!cli_edhoc_handshake_is_initialized(state) ||
      !com_writable_buffer_is_writable(message_3_or_error) ||
      message_3_or_error->capacity < min_payload_capacity) {
    return message_3_compose_local_failure(
        CLI_EDHOC_MSG3_COMPOSE_ERR_INVALID_ARGS);
  }

  struct edhoc_prepended_fields prepended_fields = {
      .buffer = message_3_or_error->bytes,
      .buffer_size = message_3_or_error->capacity,
      .edhoc_message_ptr = message_3_or_error->bytes,
      .edhoc_message_size = message_3_or_error->capacity,
  };

  int edhoc_result = edhoc_prepend_connection_id(
      &prepended_fields, &state->context.private_peer_cid);
  if (edhoc_result != EDHOC_SUCCESS) {
    (void)com_edhoc_add_edhoc_error_to_response_with_description(
        &state->context, "Failed to prepend connection id for message 3",
        message_3_or_error);
    return message_3_compose_protocol_failure(
        CLI_EDHOC_MSG3_COMPOSE_ERR_CONNECTION_ID_PREPEND_FAILED,
        *message_3_or_error);
  }

  size_t message3_len = 0;
  edhoc_result = edhoc_message_3_compose(
      &state->context, prepended_fields.edhoc_message_ptr,
      prepended_fields.edhoc_message_size, &message3_len);
  if (edhoc_result != EDHOC_SUCCESS) {
    (void)com_edhoc_add_edhoc_error_to_response_with_description(
        &state->context, "Failed to compose EDHOC message 3",
        message_3_or_error);
    return message_3_compose_protocol_failure(
        CLI_EDHOC_MSG3_COMPOSE_ERR_EDHOC_MESSAGE_3_COMPOSE_FAILED,
        *message_3_or_error);
  }

  prepended_fields.edhoc_message_size = message3_len;
  edhoc_result = edhoc_prepend_recalculate_size(&prepended_fields);
  if (edhoc_result != EDHOC_SUCCESS) {
    (void)com_edhoc_add_edhoc_error_to_response_with_description(
        &state->context, "Failed to recalculate prepended message size",
        message_3_or_error);
    return message_3_compose_protocol_failure(
        CLI_EDHOC_MSG3_COMPOSE_ERR_PREPEND_RECALCULATION_FAILED,
        *message_3_or_error);
  }

  message_3_or_error->length = prepended_fields.buffer_size;
  return message_3_compose_ok(*message_3_or_error);
}
