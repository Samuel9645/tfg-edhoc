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

/**
 * @brief Error codes for EDHOC server handshake operations.
 */
typedef enum edh_srv_message_1_handler_status {
  EDH_SRV_MSG1_HDL_OK = 0,
  EDH_SRV_MSG1_HDL_ERR_INVALID_ARGS,
  EDH_SRV_MSG1_HDL_ERR_PAYLOAD_TOO_LARGE,
  EDH_SRV_MSG1_HDL_ERR_PREFIX_MISSING,
  EDH_SRV_MSG1_HDL_ERR_CALLOC_FAILED,
  EDH_SRV_MSG1_HDL_ERR_EDHOC_CONTEXT_SETUP_FAILED,
  EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED,
  EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED,
} edh_srv_message_1_handler_status_t;

/**
 * @brief Result of handling EDHOC Message 1, including status and allocated
 * context on success.
 */
typedef struct ehd_srv_message_1_handler_result {
  /** Status of the handshake operation. */
  edh_srv_message_1_handler_status_t status;
  /** Pointer to the allocated EDHOC context on success, NULL on failure. */
  struct edhoc_context* edhoc_ctx;
} ehd_srv_message_1_handler_result_t;

ehd_srv_message_1_handler_result_t edh_srv_message_1_handler_ok(
    struct edhoc_context* edhoc_ctx);

ehd_srv_message_1_handler_result_t edh_srv_message_1_handler_failure(
    edh_srv_message_1_handler_status_t status);

#endif  // EDHOC_SERVER_MESSAGE_1_RESULT_H