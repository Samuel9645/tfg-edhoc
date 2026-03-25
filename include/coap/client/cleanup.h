#ifndef COAP_CLIENT_CLEANUP_H_
#define COAP_CLIENT_CLEANUP_H_

#include "coap/client/exchange.h"
#include "common/cleanup.h"
#include "edhoc/client/handshake.h"

/**
 * @brief Client-specific resource bundle that extends generic session cleanup
 * data with EDHOC client flow/exchange ownership.
 */
typedef struct {
  /** Generic CoAP/EDHOC session resources. */
  session_resources_t session_resources;

  /** Client EDHOC flow state owned by the caller. */
  client_edhoc_flow_t flow;

  /** Exchange state owned by the caller. */
  coap_client_exchange_t exchange;
} coap_client_session_resources_t;

/**
 * @brief Cleanup all resources allocated for the CoAP EDHOC client path.
 *
 * @param resources Pointer to coap_client_session_resources_t containing
 * generic and client-specific resources.
 *
 * @note Preferred single cleanup entry point for client flow/exchange state.
 *
 * @warning When using this function, avoid manual calls to
 * client_edhoc_flow_deinit() for the same resources in the same control path.
 */
void coap_client_cleanup_resources(coap_client_session_resources_t* resources);

#endif  // COAP_CLIENT_CLEANUP_H_