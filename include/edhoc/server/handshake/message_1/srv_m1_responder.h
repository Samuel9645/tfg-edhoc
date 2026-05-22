/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Logic for responding to Message 1 (process Message 1 and compose
 * Message 2)
 * @see [RFC 9528
 * A.2.1](https://datatracker.ietf.org/doc/html/rfc9528/#name-the-forward-message-flow)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_HANDLER_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_HANDLER_H_

#include "common/com_data_models.h"
#include "edhoc/common/com_edhoc_setup_context.h"
#include "edhoc/server/handshake/message_1/srv_m1_responder_result.h"

// TODO: maybe inline this struct
struct srv_edhoc_message_1_responder_request {
  struct com_readonly_buffer message_1;
};

/**
 * @brief Handle EDHOC Message 1 and compose Message 2.
 *
 * @param[in] request Session/request metadata for Message 1 responder,
 * including raw CoAP payload and credentials.
 * @param[in] edhoc_parameters Parameters to use when creating the EDHOC
 * context in the processing phase
 * @param[in] response Response buffer metadata for Message 2.
 * @return Struct containing status code, allocated EDHOC context and Message 2
 * in the buffer on success, or error code, NULL context and error buffer on
 * failure.
 * @warning This function dynamically allocates the EDHOC context using
 * calloc(). On success, the caller assumes ownership of this memory and is
 * responsible for freeing it. On failure, the function safely cleans up after
 * itself and returns a NULL pointer.
 */
struct srv_edhoc_message_1_responder_result srv_edhoc_respond_to_message_1(
    struct srv_edhoc_message_1_responder_request request,
    struct com_edhoc_parameters edhoc_parameters,
    struct com_writable_buffer response);

const char* srv_edhoc_message_1_responder_status_code_to_string(
    enum srv_edhoc_message_1_responder_status status);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_HANDLER_H_
