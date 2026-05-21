/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 21/05/2026
 * @brief Declarations for the function to bind the OSCORE session to the
 * current CoAP context using EDHOC
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef COAP_SERVER_OSCORE_SRV_OSCORE_BIND_CONTEXT_H_
#define COAP_SERVER_OSCORE_SRV_OSCORE_BIND_CONTEXT_H_

#include <coap3/coap.h>
#include <edhoc.h>

#include "coap/common/com_coap_status.h"

enum status_coap srv_oscore_bind_session(coap_context_t* coap_context,
                                         struct edhoc_context* edhoc_context);

#endif  // COAP_SERVER_OSCORE_SRV_OSCORE_BIND_CONTEXT_H_