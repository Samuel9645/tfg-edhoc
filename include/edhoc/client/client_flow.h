#ifndef EDHOC_CLIENT_CLIENT_FLOW_H_
#define EDHOC_CLIENT_CLIENT_FLOW_H_

#include <edhoc.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief EDHOC client flow state.
 */
typedef struct client_edhoc_flow_t {
  /** Internal EDHOC protocol context used across message 1-4 operations. */
  struct edhoc_context context;
} client_edhoc_flow_t;

/**
 * @brief Result codes for EDHOC client flow operations.
 */
typedef enum {
  CLIENT_EDHOC_FLOW_SUCCESS = 0,
  CLIENT_EDHOC_FLOW_INVALID_ARGUMENT,
  CLIENT_EDHOC_FLOW_MEMORY_ALLOCATION_FAILED,
  CLIENT_EDHOC_FLOW_CONTEXT_SETUP_FAILED,
  CLIENT_EDHOC_FLOW_MESSAGE_1_COMPOSE_FAILED,
  CLIENT_EDHOC_FLOW_MESSAGE_2_PROCESS_FAILED,
  CLIENT_EDHOC_FLOW_CONNECTION_ID_PREPEND_FAILED,
  CLIENT_EDHOC_FLOW_MESSAGE_3_COMPOSE_FAILED,
  CLIENT_EDHOC_FLOW_PREPEND_RECALCULATION_FAILED,
  CLIENT_EDHOC_FLOW_MESSAGE_4_PROCESS_FAILED,
} client_edhoc_flow_status_t;

/**
 * @brief Initialize EDHOC client flow context.
 *
 * @param[in,out] flow Caller-owned flow state.
 * @return CLIENT_EDHOC_FLOW_SUCCESS on success, descriptive error otherwise.
 *
 * @note This function does not allocate memory. The caller owns flow storage
 * and must keep it alive for all flow operations.
 */
client_edhoc_flow_status_t client_edhoc_flow_init(client_edhoc_flow_t* flow);

/**
 * @brief Compose EDHOC Message 1 including CBOR marker.
 *
 * @param[in] flow Initialized flow.
 * @param[in] payload_capacity Capacity of payload buffer.
 * @param[out] payload Output payload buffer.
 * @param[out] payload_len Number of bytes written.
 * @return CLIENT_EDHOC_FLOW_SUCCESS on success, descriptive error otherwise.
 */
client_edhoc_flow_status_t client_edhoc_flow_compose_message_1(
    client_edhoc_flow_t* flow, size_t payload_capacity, uint8_t* payload,
    size_t* payload_len);

/**
 * @brief Process EDHOC Message 2.
 *
 * @param[in] flow Initialized flow.
 * @param[in] payload Message payload.
 * @param[in] payload_len Payload length.
 * @return CLIENT_EDHOC_FLOW_SUCCESS on success, descriptive error otherwise.
 */
client_edhoc_flow_status_t client_edhoc_flow_process_message_2(
    client_edhoc_flow_t* flow, const uint8_t* payload, size_t payload_len);

/**
 * @brief Compose EDHOC Message 3 with prepended connection ID.
 *
 * @param[in] flow Initialized flow.
 * @param[in] payload_capacity Capacity of payload buffer.
 * @param[out] payload Output payload buffer.
 * @param[out] payload_len Number of bytes written.
 * @return CLIENT_EDHOC_FLOW_SUCCESS on success, descriptive error otherwise.
 */
client_edhoc_flow_status_t client_edhoc_flow_compose_message_3(
    client_edhoc_flow_t* flow, size_t payload_capacity, uint8_t* payload,
    size_t* payload_len);

/**
 * @brief Process EDHOC Message 4.
 *
 * @param[in] flow Initialized flow.
 * @param[in] payload Message payload.
 * @param[in] payload_len Payload length.
 * @return CLIENT_EDHOC_FLOW_SUCCESS on success, descriptive error otherwise.
 */
client_edhoc_flow_status_t client_edhoc_flow_process_message_4(
    client_edhoc_flow_t* flow, const uint8_t* payload, size_t payload_len);

/**
 * @brief Destroy EDHOC client flow context.
 *
 * @param[in,out] flow Caller-owned flow state to clean up.
 */
void client_edhoc_flow_deinit(client_edhoc_flow_t* flow);

#endif  // EDHOC_CLIENT_CLIENT_FLOW_H_
