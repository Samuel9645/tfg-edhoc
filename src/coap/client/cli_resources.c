#include "coap/client/cli_resources.h"

void cli_coap_cleanup_resources(struct cli_coap_session_resources* resources) {
  if (resources == NULL) {
    return;
  }

  edhoc_context_deinit(resources->edhoc_context);
  com_cleanup_resources(&resources->common_resources);
}