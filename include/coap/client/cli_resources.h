#ifndef COAP_CLIENT_CLI_RESOURCES_H_
#define COAP_CLIENT_CLI_RESOURCES_H_

#include "coap/client/cli_exchange.h"
#include "common/com_session_resources.h"
#include "edhoc/client/handshake/common/cli_state.h"

struct cli_coap_session_resources {
  struct com_session_resources common_resources;
  coap_session_t* coap_session;
  struct cli_edhoc_handshake handshake;
  struct cli_coap_exchange* exchange;
};

/**
 * @brief Cleanup all resources allocated for the CoAP EDHOC client path.
 *
 * @param resources Pointer to coap_client_session_resources_t containing
 * generic and client-specific resources.
 *
 * @note Preferred single cleanup entry point for client handshake/exchange
 * state.
 */
void cli_coap_cleanup_resources(struct cli_coap_session_resources* resources);

#endif  // COAP_CLIENT_CLI_RESOURCES_H_