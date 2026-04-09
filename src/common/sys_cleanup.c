#include "common/sys_cleanup.h"

void com_cleanup_resources(struct com_session_resources* resources) {
  if (resources == NULL) {
    return;
  }

  if (resources->options) {
    coap_delete_optlist(resources->options);
    resources->options = NULL;
  }
  if (resources->coap_session) {
    coap_session_release(resources->coap_session);
    resources->coap_session = NULL;
  }
  if (resources->coap_context) {
    coap_free_context(resources->coap_context);
    resources->coap_context = NULL;
  }
  if (resources->pdu) {
    coap_delete_pdu(resources->pdu);
    resources->pdu = NULL;
  }
  if (resources->edhoc_current_context) {
    edhoc_context_deinit(resources->edhoc_current_context);
    resources->edhoc_current_context = NULL;
  }
}