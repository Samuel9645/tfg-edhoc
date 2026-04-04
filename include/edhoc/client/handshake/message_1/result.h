/**
 * @file result.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Result codes for client handshake Message 1 handling.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_RESULT_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_RESULT_H_

#include "common/data_models.h"

/**
 * @brief Status codes for client handshake Message 1 composition.
 */
typedef enum edh_cli_message_1_compose_status {
  EDH_CLI_MSG1_COMPOSE_OK = 0,
  EDH_CLI_MSG1_COMPOSE_ERR_INVALID_ARGS,
  EDH_CLI_MSG1_COMPOSE_ERR_PAYLOAD_TOO_SMALL,
  EDH_CLI_MSG1_COMPOSE_ERR_EDHOC_MESSAGE_1_COMPOSE_FAILED,
} edh_cli_message_1_compose_status_t;

/**
 * @brief Result wrapper for Message 1 compose operation.
 *
 * @note output is a non-owning view into caller-provided buffer.
 */
typedef struct edh_cli_message_1_result {
  /** Operation status code. */
  edh_cli_message_1_compose_status_t status;

  /** Output payload view (same buffer passed by caller). */
  com_writable_buffer_t output;
} edh_cli_message_1_result_t;

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_RESULT_H_
