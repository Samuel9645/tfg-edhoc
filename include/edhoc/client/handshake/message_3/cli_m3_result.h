/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Result codes for client handshake Message 3 composition.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_CLI_M3_RESULT_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_CLI_M3_RESULT_H_

#include "common/com_data_models.h"

enum cli_edhoc_message_3_compose_status {
  CLI_EDHOC_MSG3_COMPOSE_OK = 0,
  CLI_EDHOC_MSG3_COMPOSE_ERR_INVALID_ARGS,
  CLI_EDHOC_MSG3_COMPOSE_ERR_CONNECTION_ID_PREPEND_FAILED,
  CLI_EDHOC_MSG3_COMPOSE_ERR_EDHOC_MESSAGE_3_COMPOSE_FAILED,
  CLI_EDHOC_MSG3_COMPOSE_ERR_PREPEND_RECALCULATION_FAILED,
};

struct cli_edhoc_message_3_compose_result {
  enum cli_edhoc_message_3_compose_status status;
  struct com_writable_buffer output;
};

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_CLI_M3_RESULT_H_
