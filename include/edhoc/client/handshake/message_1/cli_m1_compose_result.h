/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 22/04/2026
 * @brief Results of Message 1 composition on client side
 * @see [RFC
 * 9528 5.2.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-initiator-composition-of-me)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_CLI_M1_COMPOSE_RESULT_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_CLI_M1_COMPOSE_RESULT_H_

#include "common/com_data_models.h"

enum cli_edhoc_message_1_compose_status {
  CLI_EDHOC_MSG1_COMPOSE_OK = 0,
  CLI_EDHOC_MSG1_COMPOSE_ERR_INVALID_COMPOSE_BUFFER,
  CLI_EDHOC_MSG1_COMPOSE_ERR_EDHOC_MESSAGE_1_COMPOSE,
  CLI_EDHOC_MSG1_COMPOSE_ERR_EMPTY_COMPOSE,
};

struct cli_edhoc_message_1_compose_result {
  const enum cli_edhoc_message_1_compose_status status;
  const struct com_readonly_buffer buffer;
};

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_CLI_M1_COMPOSE_RESULT_H_
