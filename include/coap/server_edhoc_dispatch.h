#ifndef COAP_SERVER_EDHOC_DISPATCH_H_
#define COAP_SERVER_EDHOC_DISPATCH_H_

#include <coap3/coap.h>

/**
 * @brief Dispatch CoAP EDHOC POST requests to the right message handler.
 *
 * @param[in] session Active CoAP session.
 * @param[in] request Incoming request PDU.
 * @param[in,out] response Outgoing response PDU.
 */
void coap_server_dispatch_edhoc_post(coap_session_t* session,
                                     const coap_pdu_t* request,
                                     coap_pdu_t* response);

#endif  // COAP_SERVER_EDHOC_DISPATCH_H_
