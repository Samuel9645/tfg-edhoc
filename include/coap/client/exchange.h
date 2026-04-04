#ifndef COAP_CLIENT_EXCHANGE_H_
#define COAP_CLIENT_EXCHANGE_H_

#include <coap3/coap.h>
#include <stdbool.h>
#include <stdint.h>

#include "coap/common/status.h"
#include "coap/config.h"
#include "common/data_models.h"

/**
 * @brief Input data used to initialize client exchange state.
 * Also embedded in coap_client_exchange_t as session data.
 */
typedef struct cp_cli_exchange_session_data {
  /** Context used by wait loop and response callback registration. */
  coap_context_t* context;

  /** Session used to send requests and store app-data pointer. */
  coap_session_t* session;

  /** Cached URI used for outgoing EDHOC POST requests. */
  coap_uri_t uri;

  /** Cached destination address associated with uri. */
  coap_address_t destination;
} cp_cli_exchange_session_data_t;

/**
 * @brief Validate session data has required pointer fields.
 *
 * @param[in] session_data Session and endpoint data.
 * @return true if context and session are non-NULL, false otherwise.
 */
bool cp_cli_exchange_session_data_is_valid(
    const cp_cli_exchange_session_data_t* session_data);

/**
 * @brief CoAP exchange state for EDHOC client messages.
 */
typedef struct cp_cli_exchange {
  /** Shared session data (transport and endpoint info). */
  cp_cli_exchange_session_data_t session_data;

  /** Response-ready flag set by response callback. */
  bool have_response;

  /** Internal receive buffer populated by response callback. */
  uint8_t incoming_message[CP_CFG_MAX_PDU_SIZE];

  /** Number of valid bytes currently stored in incoming_message. */
  size_t incoming_message_length;

  /** Last non-empty response code received for this exchange. */
  coap_pdu_code_t last_response_code;
} cp_cli_exchange_t;

/**
 * @brief Input data used to send one EDHOC request message.
 */
typedef struct cp_cli_exchange_request_data {
  /** Payload pointer and length for this outgoing request. */
  com_readonly_buffer_t request_data;

  /** CoAP content-format option value to attach to the request. */
  cp_cfg_content_format_edhoc_values_t content_format;
} cp_cli_exchange_request_data_t;

bool cp_cli_exchange_request_data_is_valid(
    const cp_cli_exchange_request_data_t* request_data);

/**
 * @brief Check if response message size fits in buffer capacity.
 *
 * @param[in] message_length Size of incoming message.
 * @param[in] capacity Available buffer capacity.
 * @return true if message fits, false otherwise.
 */
bool cp_cli_exchange_response_size_fits(size_t message_length, size_t capacity);

/**
 * @brief Initialize exchange state and register response handler.
 *
 * @param[in] session_data Session and endpoint data required for init.
 * @param[out] exchange Exchange state storage provided by caller.
 * @return CCOM_STATUS_SUCCESS on success, CCOM_ERROR on
 * failure.
 */
cp_status_t cp_cli_init_exchange(
    const cp_cli_exchange_session_data_t* session_data,
    cp_cli_exchange_t* exchange);

/**
 * @brief Send EDHOC payload in a CoAP POST request.
 *
 * @param[in] exchange Initialized exchange state.
 * @param[in] request_data EDHOC request payload and content format.
 * @return CCOM_STATUS_SUCCESS on success, CCOM_ERROR on
 * failure.
 */
cp_status_t cp_cli_exchange_send(
    cp_cli_exchange_t* exchange,
    const cp_cli_exchange_request_data_t* request_data);

/**
 * @brief Wait for response and copy payload to caller buffer.
 *
 * @param[in,out] exchange Initialized exchange state.
 * @param[out] response_data Response output buffer metadata.
 * @return CCOM_STATUS_SUCCESS on 2.04 Changed responses,
 * CCOM_ERROR on transport failures or CoAP error responses.
 *
 * @note For CoAP error responses, the EDHOC error payload is still copied to
 * response_data when present and valid.
 */
cp_status_t cp_cli_exchange_wait_and_get(cp_cli_exchange_t* exchange,
                                         com_writable_buffer_t* response_data);

/**
 * @brief Reset response state before next request.
 *
 * @param[in,out] exchange Initialized exchange state.
 */
void cp_cli_exchange_reset(cp_cli_exchange_t* exchange);

#endif  // COAP_CLIENT_EXCHANGE_H_
