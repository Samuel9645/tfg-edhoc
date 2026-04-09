/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief EDHOC results type definition to integrate with any layer above the
 * EDHOC library, such as CoAP server handlers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_MESSAGE_1_RESULT_H
#define EDHOC_SERVER_MESSAGE_1_RESULT_H

enum edh_srv_message_1_handler_status {
  EDH_SRV_MSG1_HDL_OK = 0,
  EDH_SRV_MSG1_HDL_ERR_NULL_CREDENTIALS,
  EDH_SRV_MSG1_HDL_ERR_INVALID_REQUEST_BUFFER,
  EDH_SRV_MSG1_HDL_ERR_INVALID_RESPONSE_BUFFER,
  EDH_SRV_MSG1_HDL_ERR_CALLOC_FAILED,
  EDH_SRV_MSG1_HDL_ERR_EDHOC_CONTEXT_SETUP_FAILED,
  EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED,
  EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED,
  EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_EMPTY,
};

struct edh_srv_message_1_handler_result {
  enum edh_srv_message_1_handler_status status;
  /**
   * Pointer to the allocated EDHOC context on success, NULL on failure.
   * @warning The caller is responsible for freeing the allocated context using
   * edhoc_context_deinit() to clean up the library context and then free() when
   * no longer needed.
   */
  struct edhoc_context* edhoc_ctx;
};

#endif  // EDHOC_SERVER_MESSAGE_1_RESULT_H