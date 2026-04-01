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
typedef enum edh_srv_hnd_m1_status {
  EDH_SERV_HND_M1_OK = 0,
  EDH_SERV_HND_M1_ERR_INVALID_ARGS,
  EDH_SERV_HND_M1_ERR_PAYLOAD_TOO_LARGE,
  EDH_SERV_HND_M1_ERR_PREFIX_MISSING,
  EDH_SERV_HND_M1_ERR_CALLOC_FAILED,
  EDH_SERV_HND_M1_ERR_COAP_SESSION_ALREADY_HAS_DATA,
  EDH_SERV_HND_M1_ERR_EDHOC_CONTEXT_SETUP_FAILED,
  EDH_SERV_HND_M1_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED,
  EDH_SERV_HND_M1_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED,
} edh_srv_hnd_m1_status_t;

/**
 * @brief Result of handling EDHOC Message 1, including status and allocated
 * context on success.
 */
typedef struct edh_srv_hnd_m1_result {
  /** Status of the handshake operation. */
  edh_srv_hnd_m1_status_t status;
  /** Pointer to the allocated EDHOC context on success, NULL on failure. */
  struct edhoc_context* edhoc_ctx;
} edh_srv_hnd_m1_result_t;

edh_srv_hnd_m1_result_t edh_srv_hnd_m1_ok(struct edhoc_context* edhoc_ctx);

edh_srv_hnd_m1_result_t edh_srv_hnd_m1_failure(edh_srv_hnd_m1_status_t status);

#endif  // EDHOC_SERVER_MESSAGE_1_RESULT_H