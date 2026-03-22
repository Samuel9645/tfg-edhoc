#ifndef COAP_CLIENT_EDHOC_EXCHANGE_H_
#define COAP_CLIENT_EDHOC_EXCHANGE_H_

#include <coap3/coap.h>
#include <stddef.h>
#include <stdint.h>

#include "coap/common/config.h"
#include "coap/common/status.h"

/**
 * @brief Opaque CoAP exchange state for EDHOC client messages.
 */
typedef struct client_edhoc_exchange_t client_edhoc_exchange_t;

/**
 * @brief Initialize exchange state and register response handler.
 *
 * @param[in] context Active CoAP context.
 * @param[in] session Active CoAP session.
 * @param[in] uri Parsed destination URI.
 * @param[in] destination Parsed destination address.
 * @param[out] exchange_out Allocated exchange state.
 * @return COAP_STATUS_SUCCESS on success, COAP_STATUS_ERROR on failure.
 *
 * @warning Allocates memory with calloc(). Caller must call
 * client_edhoc_exchange_deinit() before application exit to free allocated
 * resources.
 */
coap_status_result_t client_edhoc_exchange_init(
    coap_context_t* context, coap_session_t* session, const coap_uri_t* uri,
    const coap_address_t* destination, client_edhoc_exchange_t** exchange_out);

/**
 * @brief Send EDHOC payload in a CoAP POST request.
 *
 * @param[in] exchange Initialized exchange state.
 * @param[in] payload Payload bytes.
 * @param[in] payload_len Payload length.
 * @param[in] content_format Content format option for request.
 * @return COAP_STATUS_SUCCESS on success, COAP_STATUS_ERROR on failure.
 */
coap_status_result_t client_edhoc_exchange_send(
    client_edhoc_exchange_t* exchange, const uint8_t* payload,
    size_t payload_len, content_format_edhoc_values_t content_format);

/**
 * @brief Wait for response and copy payload to caller buffer.
 *
 * @param[in,out] exchange Initialized exchange state.
 * @param[in] response_capacity Capacity of response_buffer.
 * @param[out] response_buffer Response output buffer.
 * @param[out] response_len Number of bytes copied.
 * @return COAP_STATUS_SUCCESS on success, COAP_STATUS_ERROR on failure.
 */
coap_status_result_t client_edhoc_exchange_wait_and_get(
    client_edhoc_exchange_t* exchange, size_t response_capacity,
    uint8_t* response_buffer, size_t* response_len);

/**
 * @brief Reset response state before next request.
 *
 * @param[in,out] exchange Initialized exchange state.
 */
void client_edhoc_exchange_reset(client_edhoc_exchange_t* exchange);

/**
 * @brief Destroy exchange state and unregister app-data.
 *
 * @param[in,out] exchange_ptr Exchange pointer to destroy.
 * @warning Frees memory allocated by client_edhoc_exchange_init(). Sets
 * exchange_ptr to NULL.
 */
void client_edhoc_exchange_deinit(client_edhoc_exchange_t** exchange_ptr);

#endif  // COAP_CLIENT_EDHOC_EXCHANGE_H_
