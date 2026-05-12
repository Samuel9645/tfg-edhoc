/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 22/04/2026
 * @brief Results of Message 3 composition on client side
 * @see [RFC
 * 9528 5.4.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-initiator-composition-of-mes
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_CLI_M3_COMPOSE_RESULT_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_CLI_M3_COMPOSE_RESULT_H_

#include "common/com_data_models.h"

enum cli_edhoc_message_3_compose_status {
  CLI_EDHOC_MSG3_COMPOSE_OK = 0,
  CLI_EDHOC_MSG3_COMPOSE_ERR_INVALID_COMPOSE_BUFFER,
  CLI_EDHOC_MSG3_COMPOSE_ERR_NULL_CONTEXT,
  CLI_EDHOC_MSG3_COMPOSE_ERR_EDHOC_MESSAGE_3_COMPOSE_FAILED,
  CLI_EDHOC_MSG3_COMPOSE_ERR_EMPTY_COMPOSE,
};

struct cli_edhoc_message_3_compose_result {
  const enum cli_edhoc_message_3_compose_status status;
  const struct com_readonly_buffer buffer;
};

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_CLI_M3_COMPOSE_RESULT_H_
