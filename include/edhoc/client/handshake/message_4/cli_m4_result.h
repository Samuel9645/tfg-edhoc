/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Result codes for client handshake Message 4 processing.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_RESULT_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_RESULT_H_

#include "common/data_models.h"

/**
 * @brief Status codes for client handshake Message 4 processing.
 */
typedef enum edh_cli_message_4_process_status {
  EDH_CLI_MSG4_PROCESS_OK = 0,
  EDH_CLI_MSG4_PROCESS_ERR_INVALID_ARGS,
  EDH_CLI_MSG4_PROCESS_ERR_EDHOC_MESSAGE_4_PROCESS_FAILED,
} edh_cli_message_4_process_status_t;

/**
 * @brief Result wrapper for Message 4 process operation.
 *
 * @note output is a non-owning view into caller-provided buffer.
 */
typedef struct edh_cli_message_4_result {
  /** Operation status code. */
  edh_cli_message_4_process_status_t status;

  /** Output payload view (error payload on failure, empty on success). */
  com_writable_buffer_t output;
} edh_cli_message_4_result_t;

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_RESULT_H_
