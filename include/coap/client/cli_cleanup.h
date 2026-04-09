#ifndef COAP_CLIENT_CLEANUP_H_
#define COAP_CLIENT_CLEANUP_H_

#include "coap/client/cli_exchange.h"
#include "common/sys_cleanup.h"
#include "edhoc/client/handshake/common/cli_state.h"

struct cp_cli_session_resources {
  struct com_session_resources session_resources;
  struct edh_cli_handshake handshake;
  struct cp_cli_exchange exchange;
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
void cp_cli_cleanup_resources(struct cp_cli_session_resources* resources);

#endif  // COAP_CLIENT_CLEANUP_H_