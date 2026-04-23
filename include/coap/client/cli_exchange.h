
#ifndef COAP_CLIENT_CLI_EXCHANGE_H_
#define COAP_CLIENT_CLI_EXCHANGE_H_

#include <coap3/coap.h>
#include <stdbool.h>

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
 * @brief Opaque struct to hold exchange-related data.
 * This context is initialized via cli_coap_init_exchange() and its lifecycle
 * is tied to the CoAP session. It is automatically deallocated via the
 * coap_session_set_app_data2() release callback when the session terminates.
 */
struct cli_coap_exchange;

/**
 * @brief Initializes the exchange state, binds session data, and registers
 * handlers.
 *
 * This function handles the dynamic allocation of the exchange edhoc_context
 * and orchestrates the necessary libcoap registrations.
 *
 * @param[in] session_data Session and endpoint metadata required for
 * initialization.
 * @param[in] response_buffer The writable buffer data, designated to store the
 * incoming CoAP response.
 * @return A pointer to the initialized exchange on success, or NULL if
 * allocation or session binding fails.
 *
 * @warning This function allocates memory using calloc(). The memory is managed
 * by libcoap, ensure the CoAP session is eventually terminated to trigger the
 * automatic deletion callback.
 */
struct cli_coap_exchange* cli_coap_init_exchange(
    const struct cli_coap_exchange_session_data* session_data,
    struct com_writable_buffer response_buffer);

/**
 * @brief Send EDHOC payload in a CoAP POST request.
 *
 * @param[in] exchange Initialized exchange state.
 * @param[in] request_data EDHOC request payload and content format.
 * @return CP_STATUS_SUCCESS on success, CP_STATUS_FAILURE on
 * failure.
 * @note This resets the state of the exchange before sending the data
 */
enum status_coap cli_coap_exchange_send(
    struct cli_coap_exchange* exchange,
    struct cli_coap_exchange_request request_data);

struct cli_coap_wait_and_get_result {
  const enum status_coap status;
  const struct com_readonly_buffer response;
};

/**
 * @brief Wait for response and copy payload to caller buffer.
 *
 * @param[in,out] exchange Initialized exchange state.
 * @return Struct containing status code and view of response payload on
 * success, empty view on failure.
 *
 * @note For CoAP error responses, the EDHOC error payload is still copied to
 * response_data when present and valid.
 */
struct cli_coap_wait_and_get_result cli_coap_exchange_wait_and_get(
    const struct cli_coap_exchange* exchange);

#endif  // COAP_CLIENT_CLI_EXCHANGE_H_
