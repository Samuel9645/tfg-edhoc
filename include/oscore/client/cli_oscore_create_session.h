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

/**
 * @brief Create a new CoAP session protected with OSCORE using the previous one
 * @param coap_context Active CoAP context.
 * @param edhoc_context Active EDHOC context with a successfully completed
 * handshake, ready to export the OSCORE session.
 * @param destination_address Address of the remote endpoint to which the
 * session will be bound.
 * @return Pointer to the session on success, NULL on failure
 */
coap_session_t* cli_oscore_create_session(
    coap_context_t* coap_context, struct edhoc_context* edhoc_context,
    const coap_address_t* destination_address);

#endif  // COAP_SERVER_OSCORE_SRV_OSCORE_BIND_CONTEXT_H_