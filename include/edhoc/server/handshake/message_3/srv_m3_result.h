/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief EDHOC Message 3 operation result type to decouple EDHOC logic from
 * transport-level response mapping.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_RESULT_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_RESULT_H_

enum srv_edhoc_message_3_handler_status {
  EDH_MSG3_HDL_OK = 0,
  EDH_MSG3_HDL_ERR_NULL_EDHOC_CONTEXT,
  EDH_MSG3_HDL_ERR_INVALID_PARSED_MESSAGE_3,
  EDH_MSG3_HDL_ERR_INVALID_RESPONSE_BUFFER,
  EDH_MSG3_HDL_ERR_MESSAGE_3_PROCESS_FAILED,
  EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_FAILED,
  EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_EMPTY,
};

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_RESULT_H_
