/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Composition implementation for client handshake Message 1.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_1/cli_m1_compose.h"

#include "edhoc/common/constants.h"

static struct edh_cli_message_1_compose_result edh_cli_message_1_create_result(
    const enum edh_cli_message_1_compose_status status,
    const struct com_writable_buffer* message_1) {
  struct edh_cli_message_1_compose_result result = {
      .status = status,
      .output = {0},
  };
  if (message_1 != NULL) {
    result.output = *message_1;
  }
  return result;
}

struct edh_cli_message_1_compose_result edh_cli_compose_message_1(
    struct edh_cli_handshake* state, struct com_writable_buffer* message_1) {
  const size_t min_prefix_payload_capacity = 2;
  if (!edh_cli_handshake_is_initialized(state) ||
      !com_writable_buffer_is_writable(message_1)) {
    return edh_cli_message_1_create_result(
        EDH_CLI_MSG1_COMPOSE_ERR_INVALID_ARGS, message_1);
  }

  if (message_1->capacity < min_prefix_payload_capacity) {
    message_1->length = 0;
    return edh_cli_message_1_create_result(
        EDH_CLI_MSG1_COMPOSE_ERR_PAYLOAD_TOO_SMALL, message_1);
  }

  message_1->bytes[0] = EDH_COM_CBOR_TRUE;
  size_t message_len = 0;
  if (edhoc_message_1_compose(&state->context, &message_1->bytes[1],
                              message_1->capacity - 1,
                              &message_len) != EDHOC_SUCCESS) {
    message_1->length = 0;
    return edh_cli_message_1_create_result(
        EDH_CLI_MSG1_COMPOSE_ERR_EDHOC_MESSAGE_1_COMPOSE_FAILED, message_1);
  }

  message_1->length = message_len + 1;
  return edh_cli_message_1_create_result(EDH_CLI_MSG1_COMPOSE_OK, message_1);
}
