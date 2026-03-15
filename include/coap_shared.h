#ifndef COAP_SHARED_H
#define COAP_SHARED_H

#include "coap3/coap.h"

int end_coap_session(coap_optlist_t* options, coap_session_t* session,
                     coap_context_t* context);

enum {
  COAP_SHARED_USE_LIBCOAP_FOR_REQUEST_AND_SINGLE_BODY_DATA =
      COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY
};

#endif  // COAP_SHARED_H