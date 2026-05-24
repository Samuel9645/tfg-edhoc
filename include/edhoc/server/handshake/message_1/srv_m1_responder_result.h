/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief EDHOC results type definition of the Message 1 responder to integrate
 * with any layer above the EDHOC library, such as CoAP server handlers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_RESULT_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_RESULT_H_

#include "common/com_data_models.h"

enum srv_edhoc_message_1_responder_status {
  SRV_EDHOC_MSG1_RESPONDER_OK = 0,
  SRV_EDHOC_MSG1_RESPONDER_ERR_INVALID_RESPONSE_BUFFER,
  SRV_EDHOC_MSG1_RESPONDER_ERR_RECEIVED_UNEXPECTED_MESSAGE_3,
  SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_1_PARSE_FAILED,
  SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_1_PROCESS_FAILED,
  SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_2_COMPOSE_FAILED,
};

struct srv_edhoc_message_1_responder_result {
  const enum srv_edhoc_message_1_responder_status status;
  const struct com_readonly_buffer response;
};

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_RESULT_H_