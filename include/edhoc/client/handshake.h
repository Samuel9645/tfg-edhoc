#ifndef EDHOC_CLIENT_HANDSHAKE_H_
#define EDHOC_CLIENT_HANDSHAKE_H_

#include <edhoc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief EDHOC client handshake state.
 */
typedef struct edhoc_client_handshake_t {
  /** Internal EDHOC protocol context used across message 1-4 operations. */
  struct edhoc_context context;

  /** Tracks whether context setup completed successfully. */
  bool initialized;
} edhoc_client_handshake_t;

/**
 * @brief Validate EDHOC client handshake is initialized.
 *
 * @param[in] handshake Handshake state.
 * @return true if handshake is non-NULL and initialized, false otherwise.
 */
static inline bool edhoc_client_handshake_is_initialized(
    const edhoc_client_handshake_t* handshake) {
  return (handshake != NULL) && (handshake->initialized);
}

/**
 * @brief Result codes for EDHOC client handshake operations.
 */
typedef enum {
  EDHOC_CLIENT_HANDSHAKE_SUCCESS = 0,
  EDHOC_CLIENT_HANDSHAKE_INVALID_ARGUMENT,
  EDHOC_CLIENT_HANDSHAKE_MEMORY_ALLOCATION_FAILED,
  EDHOC_CLIENT_HANDSHAKE_CONTEXT_SETUP_FAILED,
  EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_COMPOSE_FAILED,
  EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_PROCESS_FAILED,
  EDHOC_CLIENT_HANDSHAKE_CONNECTION_ID_PREPEND_FAILED,
  EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_COMPOSE_FAILED,
  EDHOC_CLIENT_HANDSHAKE_PREPEND_RECALCULATION_FAILED,
  EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_PROCESS_FAILED,
} edhoc_client_handshake_status_t;

/**
 * @brief Initialize EDHOC client handshake context.
 *
 * @param[in,out] handshake Caller-owned handshake state.
 * @return EDHOC_CLIENT_HANDSHAKE_SUCCESS on success, error code otherwise.
 *
 * @note This function does not allocate memory. The caller owns handshake
 * storage and must keep it alive for all handshake operations.
 */
edhoc_client_handshake_status_t edhoc_client_handshake_init(
    edhoc_client_handshake_t* handshake);

/**
 * @brief Compose EDHOC Message 1 including CBOR marker.
 *
 * @param[in] handshake Initialized handshake.
 * @param[in] payload_capacity Capacity of payload buffer.
 * @param[out] payload Output payload buffer.
 * @param[out] payload_len Number of bytes written.
 * @return EDHOC_CLIENT_HANDSHAKE_SUCCESS on success, error code otherwise.
 */
edhoc_client_handshake_status_t edhoc_client_handshake_compose_message_1(
    edhoc_client_handshake_t* handshake, size_t payload_capacity,
    uint8_t* payload, size_t* payload_len);

/**
 * @brief Process EDHOC Message 2.
 *
 * @param[in] handshake Initialized handshake.
 * @param[in] payload Message payload.
 * @param[in] payload_len Payload length.
 * @return EDHOC_CLIENT_HANDSHAKE_SUCCESS on success, error code otherwise.
 */
edhoc_client_handshake_status_t edhoc_client_handshake_process_message_2(
    edhoc_client_handshake_t* handshake, const uint8_t* payload,
    size_t payload_len);

/**
 * @brief Compose EDHOC Message 3 with prepended connection ID.
 *
 * @param[in] handshake Initialized handshake.
 * @param[in] payload_capacity Capacity of payload buffer.
 * @param[out] payload Output payload buffer.
 * @param[out] payload_len Number of bytes written.
 * @return EDHOC_CLIENT_HANDSHAKE_SUCCESS on success, error code otherwise.
 */
edhoc_client_handshake_status_t edhoc_client_handshake_compose_message_3(
    edhoc_client_handshake_t* handshake, size_t payload_capacity,
    uint8_t* payload, size_t* payload_len);

/**
 * @brief Process EDHOC Message 4.
 *
 * @param[in] handshake Initialized handshake.
 * @param[in] payload Message payload.
 * @param[in] payload_len Payload length.
 * @return EDHOC_CLIENT_HANDSHAKE_SUCCESS on success, error code otherwise.
 */
edhoc_client_handshake_status_t edhoc_client_handshake_process_message_4(
    edhoc_client_handshake_t* handshake, const uint8_t* payload,
    size_t payload_len);

/**
 * @brief Destroy EDHOC client handshake context.
 *
 * @param[in,out] handshake Caller-owned handshake state to clean up.
 *
 * @note If handshake is managed through coap_client_session_resources_t, prefer
 * calling coap_client_cleanup_resources() instead of invoking this function
 * directly.
 */
void edhoc_client_handshake_deinit(edhoc_client_handshake_t* handshake);

#endif  // EDHOC_CLIENT_HANDSHAKE_H_
