#ifndef COAP_CLIENT_CLEANUP_H_
#define COAP_CLIENT_CLEANUP_H_

#include "coap/client/exchange.h"
#include "common/cleanup.h"
#include "edhoc/client/handshake/common/cli_state.h"

/**
 * @brief Client-specific resource bundle that extends generic session cleanup
 * data with EDHOC client handshake/exchange ownership.
 */
typedef struct {
  /** Generic CoAP/EDHOC session resources. */
  com_session_resources_t session_resources;

  /** Client EDHOC handshake state owned by the caller. */
  edh_cli_handshake_t handshake;

  /** Exchange state owned by the caller. */
  cp_cli_exchange_t exchange;
} cp_cli_session_resources_t;

/**
 * @brief Cleanup all resources allocated for the CoAP EDHOC client path.
 *
 * @param resources Pointer to coap_client_session_resources_t containing
 * generic and client-specific resources.
 *
 * @note Preferred single cleanup entry point for client handshake/exchange
 * state.
 */
void cp_cli_cleanup_resources(cp_cli_session_resources_t* resources);

#endif  // COAP_CLIENT_CLEANUP_H_