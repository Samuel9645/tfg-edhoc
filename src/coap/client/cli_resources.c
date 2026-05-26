// ReSharper disable CppClassNeverUsed
#include "coap/client/cli_resources.h"

#include <string.h>

#include "common/com_session_resources.h"
#include "edhoc/common/com_edhoc_manage_context.h"

void cli_reset_edhoc_context(struct cli_resources* resources) {
  if (resources == NULL) {
    coap_log_err("cli-reset-edhoc-context resource is NULL\n");
    return;
  }
  edhoc_context_deinit(&resources->edhoc_context);
}

void cli_cleanup_resources(struct cli_resources* resources) {
  if (resources == NULL) {
    return;
  }

  cli_coap_cleanup_exchange(resources->exchange);
  cli_reset_edhoc_context(resources);
}

struct com_writable_buffer cli_resources_get_payload(
    struct cli_resources* resources) {
  if (resources == NULL) {
    coap_log_err("cli-resources-get-payload resource is NULL\n");
    return (struct com_writable_buffer){0};
  }
  return (struct com_writable_buffer){.bytes = resources->payload,
                                      .capacity = sizeof(resources->payload)};
}
