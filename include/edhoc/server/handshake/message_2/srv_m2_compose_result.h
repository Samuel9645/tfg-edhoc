/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Result structure for Message 2 composition on the responder side
 * @see [RFC
 * 9528 5.3.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-composition-of-me)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_2_SRV_M2_COMPOSE_RESULT_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_2_SRV_M2_COMPOSE_RESULT_H_
#include "common/com_data_models.h"

enum srv_edhoc_message_2_compose_status {
  SRV_EDHOC_MSG2_COMPOSE_OK = 0,
  SRV_EDHOC_MSG2_COMPOSE_ERR_INVALID_COMPOSE_BUFFER,
  SRV_EDHOC_MSG2_COMPOSE_ERR_NULL_CONTEXT,
  SRV_EDHOC_MSG2_COMPOSE_ERR_COMPOSE,
  SRV_EDHOC_MSG2_COMPOSE_ERR_EMPTY_COMPOSE,
  SRV_EDHOC_MSG2_COMPOSE_ERR_BUFFER_CONVERSION,
};

struct srv_edhoc_message_2_compose_result {
  enum srv_edhoc_message_2_compose_status status;
  struct com_readonly_buffer compose_buffer;
};

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_2_SRV_M2_COMPOSE_RESULT_H_