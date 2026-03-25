#include "coap/client/cleanup.h"

void coap_client_cleanup_resources(coap_client_session_resources_t* resources) {
  if (resources == NULL) {
    return;
  }

  client_edhoc_flow_deinit(&resources->flow);
  tfg_common_cleanup_resources(&resources->session_resources);
}