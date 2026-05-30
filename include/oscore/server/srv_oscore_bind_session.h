/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 21/05/2026
 * @brief Declarations for the function to bind the OSCORE session to the
 * current CoAP context using EDHOC
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef COAP_SERVER_OSCORE_SRV_OSCORE_BIND_SESSION_H_
#define COAP_SERVER_OSCORE_SRV_OSCORE_BIND_SESSION_H_

#include <coap3/coap.h>
#include <edhoc.h>

#include "coap/common/com_coap_status.h"

/**
 * @brief Protects the current context with OSCORE using the session derived
 * from the provided EDHOC context, and binds it to the current CoAP context.
 * @param[in] coap_context Active CoAP context.
 * @param[in] edhoc_context Active EDHOC context with a successfully completed
 * handshake, ready to export the OSCORE session.
 * @return Status indicating the result of the operation
 */
enum status_coap srv_oscore_bind_session(coap_context_t* coap_context,
                                         struct edhoc_context* edhoc_context);

#endif  // COAP_SERVER_OSCORE_SRV_OSCORE_BIND_SESSION_H_