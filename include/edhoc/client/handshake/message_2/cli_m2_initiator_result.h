/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 22/04/2026
 * @brief Results of initiator response to Message 2
 * @see [RFC
 * 9528 5.2.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-initiator-composition-of-me)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_INITIATOR_RESULT_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_INITIATOR_RESULT_H_

#include "common/com_data_models.h"

enum cli_edhoc_message_2_initiator_status {
  CLI_EDHOC_MSG2_INITIATOR_OK = 0,
  CLI_EDHOC_MSG2_INITIATOR_ERR_INVALID_RESPONSE_BUFFER,
  CLI_EDHOC_MSG2_INITIATOR_ERR_MESSAGE_2_PROCESS,
  CLI_EDHOC_MSG2_INITIATOR_ERR_MESSAGE_3_COMPOSE,
};

struct cli_edhoc_message_2_responder_result {
  const enum cli_edhoc_message_2_initiator_status status;
  const struct com_readonly_buffer buffer;
};

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_INITIATOR_RESULT_H_
