/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Result codes for client handshake Message 3 composition.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_RESULT_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_RESULT_H_

#include "common/data_models.h"

/**
 * @brief Status codes for client handshake Message 3 composition.
 */
typedef enum edh_cli_message_3_compose_status {
  EDH_CLI_MSG3_COMPOSE_OK = 0,
  EDH_CLI_MSG3_COMPOSE_ERR_INVALID_ARGS,
  EDH_CLI_MSG3_COMPOSE_ERR_CONNECTION_ID_PREPEND_FAILED,
  EDH_CLI_MSG3_COMPOSE_ERR_EDHOC_MESSAGE_3_COMPOSE_FAILED,
  EDH_CLI_MSG3_COMPOSE_ERR_PREPEND_RECALCULATION_FAILED,
} edh_cli_message_3_compose_status_t;

/**
 * @brief Result wrapper for Message 3 compose operation.
 *
 * @note output is a non-owning view into caller-provided buffer.
 */
typedef struct edh_cli_message_3_result {
  /** Operation status code. */
  edh_cli_message_3_compose_status_t status;

  /** Output payload view (Message 3 on success, EDHOC error on failure). */
  com_writable_buffer_t output;
} edh_cli_message_3_result_t;

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_RESULT_H_
