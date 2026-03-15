#include "coap_shared.h"

int end_coap_session(coap_optlist_t* options, coap_session_t* session,
                     coap_context_t* context) {
  coap_delete_optlist(options);
  coap_session_release(session);
  coap_free_context(context);
  coap_cleanup();
  return -1;
}
