#ifndef COAP_SHARED_H
#define COAP_SHARED_H

#include "coap3/coap.h"

int end_coap_session(coap_optlist_t* options, coap_session_t* session,
                     coap_context_t* context);

extern const u_int32_t USE_LIBCOAP_FOR_REQUEST_AND_SINGLE_BODY_DATA;

#endif  // COAP_SHARED_H