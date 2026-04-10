/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief EDHOC results type definition to integrate with any layer above the
 * EDHOC library, such as CoAP server handlers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_RESULT_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_RESULT_H_

enum srv_edhoc_message_1_handler_status {
  SRV_EDHOC_MSG1_HDL_OK = 0,
  SRV_EDHOC_MSG1_HDL_ERR_NULL_CREDENTIALS,
  SRV_EDHOC_MSG1_HDL_ERR_INVALID_REQUEST_BUFFER,
  SRV_EDHOC_MSG1_HDL_ERR_INVALID_RESPONSE_BUFFER,
  SRV_EDHOC_MSG1_HDL_ERR_CALLOC_FAILED,
  SRV_EDHOC_MSG1_HDL_ERR_EDHOC_CONTEXT_SETUP_FAILED,
  SRV_EDHOC_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED,
  SRV_EDHOC_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED,
  SRV_EDHOC_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_EMPTY,
};

struct srv_edhoc_message_1_handler_result {
  enum srv_edhoc_message_1_handler_status status;
  /**
   * Pointer to the allocated EDHOC context on success, NULL on failure.
   * @warning The caller is responsible for freeing the allocated context using
   * edhoc_context_deinit() to clean up the library context and then free() when
   * no longer needed.
   */
  struct edhoc_context* edhoc_ctx;
};

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_RESULT_H_