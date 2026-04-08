/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Result codes for client handshake Message 1 handling.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_RESULT_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_RESULT_H_

#include "common/data_models.h"

enum edh_cli_message_1_compose_status {
  EDH_CLI_MSG1_COMPOSE_OK = 0,
  EDH_CLI_MSG1_COMPOSE_ERR_INVALID_ARGS,
  EDH_CLI_MSG1_COMPOSE_ERR_PAYLOAD_TOO_SMALL,
  EDH_CLI_MSG1_COMPOSE_ERR_EDHOC_MESSAGE_1_COMPOSE_FAILED,
  EDH_CLI_MSG1_COMPOSE_ERR_EDHOC_PREPEND_FAILED,
  EDHOC_MSG1_COMPOSE_ERR_EDHOC_PREPEND_SIZE_CALC_FAILED,
};

struct edh_cli_message_1_compose_result {
  enum edh_cli_message_1_compose_status status;
  struct com_writable_buffer output;
};

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_RESULT_H_
