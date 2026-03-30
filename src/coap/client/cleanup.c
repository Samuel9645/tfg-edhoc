#include "coap/client/cleanup.h"

void coap_client_cleanup_resources(coap_client_session_resources_t* resources) {
  if (resources == NULL) {
    return;
  }

  edhoc_client_handshake_deinit(&resources->handshake);
  tfg_common_cleanup_resources(&resources->session_resources);
}