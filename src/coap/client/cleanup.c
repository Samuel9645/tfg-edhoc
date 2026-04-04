#include "coap/client/cleanup.h"

void cp_cli_cleanup_resources(cp_cli_session_resources_t* resources) {
  if (resources == NULL) {
    return;
  }

  edh_cli_clear_state(&resources->handshake);
  com_cleanup_resources(&resources->session_resources);
}