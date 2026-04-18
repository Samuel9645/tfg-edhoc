/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief EDHOC results type definition of the Message 3 responder to integrate
 * with any layer above the EDHOC library, such as CoAP server handlers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_RESPONDER_RESULT_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_RESPONDER_RESULT_H_

enum srv_edhoc_message_3_responder_status {
  SRV_EDHOC_MSG3_RESPONDER_OK = 0,
  SRV_EDHOC_MSG3_RESPONDER_ERR_INVALID_RESPONSE_BUFFER,
  SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_3_PROCESS,
  SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_4_COMPOSE,
};

struct srv_edhoc_message_3_responder_result {
  enum srv_edhoc_message_3_responder_status status;
};

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_RESPONDER_RESULT_H_
