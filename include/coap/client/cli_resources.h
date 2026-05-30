#ifndef COAP_CLIENT_CLI_RESOURCES_H_
#define COAP_CLIENT_CLI_RESOURCES_H_

#include "coap/client/cli_exchange.h"
#include "edhoc/client/handshake/cli_negotiate_cipher_suites.h"
#include "edhoc/common/com_edhoc_parameters.h"

struct cli_resources {
  struct edhoc_context edhoc_context;
  coap_context_t* coap_context;
  coap_session_t* exchange_session;
  coap_session_t* oscore_session;
  struct cli_coap_exchange* exchange;
  uint8_t payload[CONFIG_COAP_MAX_PDU_SIZE];
};

struct com_writable_buffer cli_resources_get_payload(
    struct cli_resources* resources);

void cli_cleanup_resources(struct cli_resources* resources);

/**
 * @brief Initializes the EDHOC context in the given resources struct with the
 * parameters provided
 * @param[in] resources Resources struct
 * @param[in] parameters New parameters to initialize the EDHOC context with.
 * @return true on success, false on failure
 * @warning This function does not clean up any existing context before
 * initializing a new one.
 */
bool cli_initialize_edhoc_context_with_parameters(
    struct cli_resources* resources, struct com_edhoc_parameters parameters);

/**
 * @brief Resets and creates a new EDHOC context using the original parameters
 * and the new cipher suites data, this is used in suite negotiation.
 * @param[in] resources Resources struct containing the EDHOC context to reset and
 * re-initialize with new parameters
 * @param[in] original_parameters Original parameters (used to copy the common data
 * such as credentials, methods...)
 * @param[in] selected_cipher_suite New selected cipher suite
 * @param[in] negotiated_suites New supported cipher suites list
 * @return true on success, false on failure
 */
bool cli_reset_edhoc_context_with_new_suites_data(
    struct cli_resources* resources,
    struct com_edhoc_parameters original_parameters,
    const struct com_edhoc_cipher_suite_details* selected_cipher_suite,
    struct cli_edhoc_renegotiation_list negotiated_suites);

#endif  // COAP_CLIENT_CLI_RESOURCES_H_