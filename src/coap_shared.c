#include "coap_shared.h"

const u_int32_t USE_LIBCOAP_FOR_REQUEST_AND_SINGLE_BODY_DATA =
    COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY;

int end_coap_session(coap_optlist_t* options, coap_session_t* session,
                     coap_context_t* context) {
  coap_delete_optlist(options);
  coap_session_release(session);
  coap_free_context(context);
  coap_cleanup();
  return -1;
}
