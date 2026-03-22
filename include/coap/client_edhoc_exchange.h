#ifndef COAP_CLIENT_EDHOC_EXCHANGE_H_
#define COAP_CLIENT_EDHOC_EXCHANGE_H_

#include <coap3/coap.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "coap/common/config.h"
#include "coap/common/data_models.h"
#include "coap/common/status.h"

/**
 * @brief CoAP exchange state for EDHOC client messages.
 */
typedef struct client_edhoc_exchange_t {
  /** Context used by wait loop and response callback registration. */
  coap_context_t* context;

  /** Session used to send requests and store app-data pointer. */
  coap_session_t* session;

  /** Cached URI used for outgoing EDHOC POST requests. */
  coap_uri_t uri;

  /** Cached destination address associated with uri. */
  coap_address_t destination;

  /** Response-ready flag set by response callback. */
  bool have_response;

  /** Internal receive buffer populated by response callback. */
  uint8_t incoming_message[MAX_PDU_SIZE];

  /** Number of valid bytes currently stored in incoming_message. */
  size_t incoming_message_len;

  /** Last non-empty response code received for this exchange. */
  coap_pdu_code_t last_response_code;
} client_edhoc_exchange_t;

/**
 * @brief Input data used to initialize client exchange state.
 */
typedef struct {
  /** CoAP context/session required for message exchange lifecycle. */
  coap_session_data_t session_data;

  /** Destination URI/address pair for outgoing EDHOC requests. */
  coap_endpoint_data_t endpoint_data;
} client_edhoc_exchange_session_data_t;

/**
 * @brief Input data used to send one EDHOC request message.
 */
typedef struct {
  /** Payload pointer and length for this outgoing request. */
  coap_request_data_t request_data;

  /** CoAP content-format option value to attach to the request. */
  content_format_edhoc_values_t content_format;
} client_edhoc_exchange_request_data_t;

/**
 * @brief Initialize exchange state and register response handler.
 *
 * @param[in] session_data Session and endpoint data required for init.
 * @param[out] exchange Exchange state storage provided by caller.
 * @return COAP_STATUS_SUCCESS on success, COAP_STATUS_ERROR on failure.
 */
coap_status_result_t client_edhoc_exchange_init(
    const client_edhoc_exchange_session_data_t* session_data,
    client_edhoc_exchange_t* exchange);

/**
 * @brief Send EDHOC payload in a CoAP POST request.
 *
 * @param[in] exchange Initialized exchange state.
 * @param[in] request_data EDHOC request payload and content format.
 * @return COAP_STATUS_SUCCESS on success, COAP_STATUS_ERROR on failure.
 */
coap_status_result_t client_edhoc_exchange_send(
    client_edhoc_exchange_t* exchange,
    const client_edhoc_exchange_request_data_t* request_data);

/**
 * @brief Wait for response and copy payload to caller buffer.
 *
 * @param[in,out] exchange Initialized exchange state.
 * @param[out] response_data Response output buffer metadata.
 * @return COAP_STATUS_SUCCESS on 2.04 Changed responses,
 * COAP_STATUS_ERROR on transport failures or CoAP error responses.
 *
 * @note For CoAP error responses, the EDHOC error payload is still copied to
 * response_data when present and valid.
 */
coap_status_result_t client_edhoc_exchange_wait_and_get(
    client_edhoc_exchange_t* exchange,
    const coap_response_data_t* response_data);

/**
 * @brief Reset response state before next request.
 *
 * @param[in,out] exchange Initialized exchange state.
 */
void client_edhoc_exchange_reset(client_edhoc_exchange_t* exchange);

#endif  // COAP_CLIENT_EDHOC_EXCHANGE_H_
