#include "coap/client_edhoc_exchange.h"
#include "common/cleanup.h"
#include "edhoc/client/client_flow.h"

void cleanup_client_resources(client_session_resources_t* resources) {
  if (resources == NULL) {
    return;
  }

  if (resources->exchange) {
    client_edhoc_exchange_deinit(&resources->exchange);
  }

  client_edhoc_flow_deinit(&resources->flow);

  cleanup_resources(&resources->session_resources);
}
