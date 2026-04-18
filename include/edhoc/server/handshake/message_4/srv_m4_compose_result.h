/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Results of Message 4 composition on the responder side.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_4_SRV_M4_COMPOSE_RESULT_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_4_SRV_M4_COMPOSE_RESULT_H_

#include "common/com_data_models.h"

enum srv_edhoc_message_4_compose_status {
  SRV_EDHOC_MSG4_COMPOSE_OK = 0,
  SRV_EDHOC_MSG4_COMPOSE_ERR_INVALID_COMPOSE_BUFFER,
  SRV_EDHOC_MSG4_COMPOSE_ERR_NULL_CONTEXT,
  SRV_EDHOC_MSG4_COMPOSE_ERR_COMPOSE_FAILED,
  SRV_EDHOC_MSG4_COMPOSE_ERR_EMPTY_COMPOSE,
  SRV_EDHOC_MSG4_COMPOSE_ERR_BUFFER_CONVERSION,
};

struct srv_edhoc_message_4_compose_result {
  enum srv_edhoc_message_4_compose_status status;
  struct com_readonly_buffer compose_buffer;
};

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_4_SRV_M4_COMPOSE_RESULT_H_

