#include "coap/client/cli_resources.h"

void cp_cli_cleanup_resources(struct cp_cli_session_resources* resources) {
  if (resources == NULL) {
    return;
  }

  edh_cli_clear_state(&resources->handshake);
  com_cleanup_resources(&resources->session_resources);
}