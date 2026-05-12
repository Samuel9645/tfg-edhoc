
#ifndef COAP_CLIENT_CLI_EXCHANGE_H_
#define COAP_CLIENT_CLI_EXCHANGE_H_

#include <coap3/coap.h>
#include <edhoc.h>
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
 * @param[in] response_buffer Metadata of the buffer (pointer and capacity) to
 * write into
 * @return A pointer to the initialized exchange on success, or NULL if
 * allocation or session binding fails.
 *
 * @warning This function allocates memory using calloc(). Ensure to call
 * cli_coap_cleanup_exchange() to free resources when the exchange is no longer
 * needed.
 */
struct cli_coap_exchange* cli_coap_init_exchange(
    const struct cli_coap_exchange_session_data* session_data,
    struct com_writable_buffer response_buffer);

/**
 * @brief Cleans up any CoAP related resources and resets session state.
 * @param exchange exchange containing the CoAP data to clean up
 * @note This function frees the exchange struct and any associated resources.
 * After calling this function, the exchange pointer should not be used.
 */
void cli_coap_cleanup_exchange(struct cli_coap_exchange* exchange);

/**
 * @brief Send EDHOC payload in a CoAP POST request.
 *
 * @param[in] exchange Initialized exchange state.
 * @param[in] request_data EDHOC request payload and content format.
 * @return STATUS_COAP_OK on success, STATUS_COAP_ERR on
 * failure.
 * @note This resets the state of the exchange before sending the data
 */
enum status_coap cli_coap_exchange_send(
    struct cli_coap_exchange* exchange,
    struct cli_coap_exchange_request request_data);

struct cli_coap_wait_and_get_result {
  const enum status_coap status;
  const struct com_readonly_buffer response;
  const bool response_is_error;
};

/**
 * @brief Wait for response and copy payload to caller buffer.
 *
 * @param[in,out] exchange Initialized exchange state.
 * @return Struct containing status code and view of response payload on
 * success with a flag telling whether the response is an error, or empty view
 * on failure.
 *
 * @note For CoAP error responses, the EDHOC error payload is still copied to
 * response_data when present and valid.
 */
struct cli_coap_wait_and_get_result cli_coap_exchange_wait_and_get(
    const struct cli_coap_exchange* exchange);

/**
 * @brief Send EDHOC Message 1 with CoAP transport wrapper (flow byte prefix).
 *
 * This function takes a pure EDHOC Message 1, adds the CoAP-specific flow
 * prefix (0xf5 byte), and sends it via the exchange. The message is sent with
 * a CoAP POST request.
 *
 * @param[in] exchange Initialized exchange state with active CoAP session.
 * @param[in] message_1 Pure EDHOC Message 1 (without any prefix).
 * @return STATUS_COAP_OK on success, STATUS_COAP_ERR on failure.
 *
 * @see [RFC 9528
 * Section 5.2](https://datatracker.ietf.org/doc/html/rfc9528/#section-5.2) for
 * EDHOC Message 1 specification.
 */
enum status_coap cli_exchange_send_message_1(
    struct cli_coap_exchange* exchange, struct com_readonly_buffer message_1);

/**
 * @brief Send EDHOC Message 3 with CoAP transport wrapper (Connection ID
 * prefix).
 *
 * This function takes a pure EDHOC Message 3, adds the server's Connection ID
 * as a prefix (extracted from the EDHOC context), and sends it via the
 * exchange. The message is sent with a CoAP POST request.
 *
 * @param[in] exchange Initialized exchange state with active CoAP session.
 * @param[in] context The EDHOC context containing the peer Connection ID.
 * @param[in] message_3 Pure EDHOC Message 3 (without any prefix).
 * @return STATUS_COAP_OK on success, STATUS_COAP_ERR on failure.
 *
 * @see [RFC 9528
 * Section 5.4](https://datatracker.ietf.org/doc/html/rfc9528/#section-5.4) for
 * EDHOC Message 3 specification.
 */
enum status_coap cli_exchange_send_message_3(
    struct cli_coap_exchange* exchange, const struct edhoc_context* context,
    struct com_readonly_buffer message_3);

#endif  // COAP_CLIENT_CLI_EXCHANGE_H_
