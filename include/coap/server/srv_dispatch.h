#ifndef COAP_SERVER_SRV_DISPATCH_H_
#define COAP_SERVER_SRV_DISPATCH_H_

#include <coap3/coap.h>
#include "coap/server/internal/srv_dispatch_engine.h"

/**
 * @brief Dispatch CoAP EDHOC POST requests to the right message handler.
 *
 * @param[in] session Active CoAP session.
 * @param[in] request Incoming request PDU.
 * @param[in] edhoc_parameters Struct containing parameters required for EDHOC
 * configuration and processing, such as credentials and supported cipher
 * suites.
 * @param[in,out] response Outgoing response PDU.
 *
 * @note Response payloads are included for both success (2.04 Changed) and
 * error responses (4.00 Bad Request, 5.00 Internal Server Error) per RFC 9528
 * A.2.3. For errors, the payload contains the EDHOC error message.
 */
void srv_coap_dispatch_edhoc_post(
    coap_session_t* session, const coap_pdu_t* request,
    struct srv_edhoc_parameters edhoc_parameters, coap_pdu_t* response);

#endif  // COAP_SERVER_SRV_DISPATCH_H_
