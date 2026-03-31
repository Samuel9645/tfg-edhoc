/**
 * @file message_1_result.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since Tue 31 Mar
 * @brief EDHOC results type definition to integrate with any layer above the
 * EDHOC library, such as CoAP server handlers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANSHAKE_RESULT_H_
#define EDHOC_SERVER_HANSHAKE_RESULT_H_

#include <edhoc.h>

/**
 * @brief Error codes for EDHOC server handshake operations.
 */
typedef enum edhoc_server_message_1_status {
  CSH_OK = 0,
  CSH_ERR_INVALID_ARGS,
  CSH_ERR_PAYLOAD_TOO_LARGE,
  CSH_ERR_PREFIX_MISSING,
  CSH_ERR_CALLOC_FAILED,
  CSH_ERR_COAP_SESSION_ALREADY_HAS_DATA,
  CSH_ERR_EDHOC_CONTEXT_SETUP_FAILED,
  CSH_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED,
  CSH_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED,
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

static inline edhoc_server_message_1_result_t edhoc_server_message_1_ok(
    struct edhoc_context* edhoc_ctx) {
  return (edhoc_server_message_1_result_t){.status = CSH_OK,
                                           .edhoc_ctx = edhoc_ctx};
}
static inline edhoc_server_message_1_result_t edhoc_server_message_1_failure(
    const edhoc_server_message_1_status_t status) {
  return (edhoc_server_message_1_result_t){.status = status, .edhoc_ctx = NULL};
}

#endif  // EDHOC_SERVER_HANSHAKE_RESULT_H_