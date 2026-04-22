#include "coap/client/cli_resources.h"

void cli_coap_cleanup_resources(struct cli_coap_session_resources* resources) {
  if (resources == NULL) {
    return;
  }

  cli_edhoc_clear_state(&resources->handshake);
  com_cleanup_resources(&resources->common_resources);
}