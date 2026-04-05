#ifndef COAP_CLIENT_EXCHANGE_H_
#define COAP_CLIENT_EXCHANGE_H_

#include <coap3/coap.h>
#include <stdbool.h>
#include <stdint.h>

#include "coap/common/status.h"
#include "coap/config.h"
#include "common/data_models.h"

struct cp_cli_exchange_session_data {
  coap_context_t* context;
  coap_session_t* session;
  coap_uri_t uri;
  coap_address_t destination;
};

/**
 * @brief Validate session data has required pointer fields.
 *
 * @param[in] session_data Session and endpoint data.
 * @return true if context and session are non-NULL, false otherwise.
 */
bool cp_cli_exchange_session_data_is_valid(
    const struct cp_cli_exchange_session_data* session_data);

/**
 * @brief CoAP exchange state for EDHOC client messages.
 */
struct cp_cli_exchange {
  struct cp_cli_exchange_session_data session_data;
  bool have_response;
  uint8_t incoming_message[CP_CFG_MAX_PDU_SIZE];
  size_t incoming_message_length;
  coap_pdu_code_t last_response_code;
};

/**
 * @brief Input data used to send one EDHOC request message.
 */
struct cp_cli_exchange_request {
  struct com_readonly_buffer buffer;
  enum cp_cfg_content_format_edhoc_values content_format;
};

bool cp_cli_exchange_request_data_is_valid(
    struct cp_cli_exchange_request request_data);

/**
 * @brief Initialize exchange state and register response handler.
 *
 * @param[in] session_data Session and endpoint data required for init.
 * @param[out] exchange Exchange state storage provided by caller.
 * @return CCOM_STATUS_SUCCESS on success, CCOM_ERROR on
 * failure.
 */
enum cp_status cp_cli_init_exchange(
    const struct cp_cli_exchange_session_data* session_data,
    struct cp_cli_exchange* exchange);

/**
 * @brief Send EDHOC payload in a CoAP POST request.
 *
 * @param[in] exchange Initialized exchange state.
 * @param[in] request_data EDHOC request payload and content format.
 * @return CCOM_STATUS_SUCCESS on success, CCOM_ERROR on
 * failure.
 */
enum cp_status cp_cli_exchange_send(
    const struct cp_cli_exchange* exchange,
    struct cp_cli_exchange_request request_data);

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
enum cp_status cp_cli_exchange_wait_and_get(
    struct cp_cli_exchange* exchange,
    struct com_writable_buffer* response_data);

/**
 * @brief Reset response state before next request.
 *
 * @param[in,out] exchange Initialized exchange state.
 */
void cp_cli_exchange_reset(struct cp_cli_exchange* exchange);

#endif  // COAP_CLIENT_EXCHANGE_H_
