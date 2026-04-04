/**
 * @file compose.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Composition implementation for client handshake Message 3.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_3/cli_m3_compose.h"

#include <edhoc_helpers.h>

#include "edhoc/client/handshake/message_3/cli_m3_errors.h"

static edh_cli_message_3_result_t edh_cli_message_3_create_result(
    const edh_cli_message_3_compose_status_t status,
    const com_writable_buffer_t* message_3) {
  edh_cli_message_3_result_t result = {
      .status = status,
      .output = {0},
  };
  if (message_3 != NULL) {
    result.output = *message_3;
  }
  return result;
}

edh_cli_message_3_result_t edh_cli_compose_message_3(
    edh_cli_handshake_t* state, com_writable_buffer_t* message_3) {
  const size_t min_payload_capacity = 1;
  if (!edh_cli_handshake_is_initialized(state) ||
      !com_writable_buffer_is_writable(message_3) ||
      message_3->capacity < min_payload_capacity) {
    return edh_cli_message_3_create_result(
        EDH_CLI_MSG3_COMPOSE_ERR_INVALID_ARGS, message_3);
  }

  struct edhoc_prepended_fields prepended_fields = {
      .buffer = message_3->bytes,
      .buffer_size = message_3->capacity,
      .edhoc_message_ptr = message_3->bytes,
      .edhoc_message_size = message_3->capacity,
  };

  int edhoc_result = edhoc_prepend_connection_id(
      &prepended_fields, &state->context.private_peer_cid);
  if (edhoc_result != EDHOC_SUCCESS) {
    edh_cli_message_3_handler_add_error(
        edhoc_result, "Failed to prepend connection id for message 3",
        message_3);
    return edh_cli_message_3_create_result(
        EDH_CLI_MSG3_COMPOSE_ERR_CONNECTION_ID_PREPEND_FAILED, message_3);
  }

  size_t message3_len = 0;
  edhoc_result = edhoc_message_3_compose(
      &state->context, prepended_fields.edhoc_message_ptr,
      prepended_fields.edhoc_message_size, &message3_len);
  if (edhoc_result != EDHOC_SUCCESS) {
    edh_cli_message_3_handler_add_error(
        edhoc_result, "Failed to compose EDHOC message 3", message_3);
    return edh_cli_message_3_create_result(
        EDH_CLI_MSG3_COMPOSE_ERR_EDHOC_MESSAGE_3_COMPOSE_FAILED, message_3);
  }

  prepended_fields.edhoc_message_size = message3_len;
  edhoc_result = edhoc_prepend_recalculate_size(&prepended_fields);
  if (edhoc_result != EDHOC_SUCCESS) {
    edh_cli_message_3_handler_add_error(
        edhoc_result, "Failed to recalculate prepended message size",
        message_3);
    return edh_cli_message_3_create_result(
        EDH_CLI_MSG3_COMPOSE_ERR_PREPEND_RECALCULATION_FAILED, message_3);
  }

  message_3->length = prepended_fields.buffer_size;
  return edh_cli_message_3_create_result(EDH_CLI_MSG3_COMPOSE_OK, message_3);
}
