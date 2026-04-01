/**
 * @file result.h
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
typedef enum edhoc_server_message_1_status {
  ESHM1_OK = 0,
  ESHM1_ERR_INVALID_ARGS,
  ESHM1_ERR_PAYLOAD_TOO_LARGE,
  ESHM1_ERR_PREFIX_MISSING,
  ESHM1_ERR_CALLOC_FAILED,
  ESHM1_ERR_COAP_SESSION_ALREADY_HAS_DATA,
  ESHM1_ERR_EDHOC_CONTEXT_SETUP_FAILED,
  ESHM1_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED,
  ESHM1_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED,
} edhoc_server_message_1_status_t;

/**
 * @brief Result of handling EDHOC Message 1, including status and allocated
 * context on success.
 */
typedef struct edhoc_server_message_1_result {
  /** Status of the handshake operation. */
  edhoc_server_message_1_status_t status;
  /** Pointer to the allocated EDHOC context on success, NULL on failure. */
  struct edhoc_context* edhoc_ctx;
} edhoc_server_message_1_result_t;

edhoc_server_message_1_result_t edhoc_server_message_1_ok(
    struct edhoc_context* edhoc_ctx);

edhoc_server_message_1_result_t edhoc_server_message_1_failure(
    edhoc_server_message_1_status_t status);

#endif  // EDHOC_SERVER_MESSAGE_1_RESULT_H