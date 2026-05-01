#ifndef COAP_CLIENT_CLI_RESOURCES_H_
#define COAP_CLIENT_CLI_RESOURCES_H_

#include "coap/client/cli_exchange.h"
#include "edhoc/common/com_edhoc_parameters.h"

struct cli_resources;

/**
 * @param context CoAP context to be used for the session, must be already
 * initialized.
 * @param session CoAP session to be used for the exchange, must be already
 * established.
 * @param uri Parsed CoAP URI for the target resource, must be already parsed
 * and valid.
 * @param address Resolved CoAP server address, must be already resolved and
 * valid.
 * @return Pointer to the client resource manager struct containing initialized
 * infrastructure and session resources, or NULL on failure.
 *
 * @warning Allocates the resources using calloc, so ensure that
 * cli_coap_cleanup_resources is called to free them when no longer needed.
 */
struct cli_resources* cli_resources_create(coap_context_t* context,
                                           coap_session_t* session,
                                           coap_uri_t uri,
                                           coap_address_t address);

/**
 * @brief Initializes or resets the EDHOC session layer on top of existing
 * infrastructure.
 *
 * This function orchestrates the setup of the client's ephemeral resources. It
 * performs a reset of any previous session state, initializes the CoAP exchange
 * handler using persistent infrastructure data, and establishes the new
 * EDHOC cryptographic context.
 *
 * @param[out] resources Pointer to the client resource manager.
 * @param[in] edhoc_parameters EDHOC configuration parameters (credentials,
 * suites, and methods) for this handshake attempt.
 *
 * @return true  If the session was successfully configured and is ready for
 * Message 1, false otherwise
 *
 * @note This function is the primary entry point for Cipher Suite
 * re-negotiation, as it allows recreating the session without closing the
 * underlying CoAP socket.
 */
bool cli_resources_setup_session(struct cli_resources* resources,
                                 struct srv_edhoc_parameters edhoc_parameters);

struct com_writable_buffer cli_resources_get_payload(
    struct cli_resources* resources);

struct edhoc_context* cli_resources_get_edhoc_context(
    struct cli_resources* resources);

struct cli_coap_exchange* cli_resources_get_exchange(
    const struct cli_resources* resources);
/**
 * @brief Cleanup all resources allocated for the CoAP EDHOC client path.
 * Called at application termination to free all CoAP and EDHOC resources.
 *
 * @param resources Pointer to cli_coap_session_resources containing
 * infrastructure and session resources.
 *
 * @note This is the final cleanup entry point; use cli_resources_reset_session
 * to reset only EDHOC state between handshake attempts.
 */
void cli_coap_cleanup_resources(struct cli_resources* resources);

#endif  // COAP_CLIENT_CLI_RESOURCES_H_