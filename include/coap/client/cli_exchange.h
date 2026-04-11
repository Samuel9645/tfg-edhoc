#ifndef COAP_CLIENT_CLI_EXCHANGE_H_
#define COAP_CLIENT_CLI_EXCHANGE_H_

#include <coap3/coap.h>
#include <stdbool.h>
#include <stdint.h>

#include "coap/coap_config.h"
#include "coap/common/com_coap_status.h"
#include "common/com_data_models.h"

struct cli_coap_exchange_session_data {
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
bool cli_coap_exchange_session_data_is_valid(
    const struct cli_coap_exchange_session_data* session_data);

// TODO: maybe use com_data_models?
struct cli_coap_exchange {
  struct cli_coap_exchange_session_data session_data;
  bool have_response;
  uint8_t incoming_message[CONFIG_COAP_MAX_PDU_SIZE];
  size_t incoming_message_length;
  coap_pdu_code_t last_response_code;
};

/**
 * @brief Input data used to send one EDHOC request message.
 */
struct cli_coap_exchange_request {
  struct com_readonly_buffer buffer;
  enum config_coap_content_format_edhoc_values content_format;
};

bool cli_coap_exchange_request_data_is_valid(
    struct cli_coap_exchange_request request_data);

/**
 * @brief Initialize exchange state and register coap_response.handler.
 *
 * @param[in] session_data Session and endpoint data required for init.
 * @param[out] exchange Exchange state storage provided by caller.
 * @return CP_STATUS_SUCCESS on success, CP_STATUS_FAILURE onon
 * failure.
 */
enum status_coap cli_coap_init_exchange(
    const struct cli_coap_exchange_session_data* session_data,
    struct cli_coap_exchange* exchange);

/**
 * @brief Send EDHOC payload in a CoAP POST request.
 *
 * @param[in] exchange Initialized exchange state.
 * @param[in] request_data EDHOC request payload and content format.
 * @return CP_STATUS_SUCCESS on success, CP_STATUS_FAILURE onon
 * failure.
 */
enum status_coap cli_coap_exchange_send(
    const struct cli_coap_exchange* exchange,
    struct cli_coap_exchange_request request_data);

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
enum status_coap cli_coap_exchange_wait_and_get(
    struct cli_coap_exchange* exchange,
    struct com_writable_buffer* response_data);

/**
 * @brief Reset response state before next request.
 *
 * @param[in,out] exchange Initialized exchange state.
 */
void cli_coap_exchange_reset(struct cli_coap_exchange* exchange);

#endif  // COAP_CLIENT_CLI_EXCHANGE_H_
