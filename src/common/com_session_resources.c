#include "common/com_session_resources.h"

void com_cleanup_resources(struct com_session_resources* resources) {
  if (resources == NULL) {
    return;
  }

  if (resources->coap_context != NULL) {
    coap_free_context(resources->coap_context);
    resources->coap_context = NULL;
  }
}