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

/**
 * @brief Handle EDHOC Message 1 and compose Message 2.
 *
 * @param[in] message_1 Buffer metadata for the EDHOC Message 1 request.
 * @param[in] edhoc_context EDHOC context created and configured by the caller.
 * @param[in] response Response buffer metadata for Message 2.
 * @return Struct containing status code and Message 2 in the buffer on success,
 * or error code and error buffer on failure.
 */
struct srv_edhoc_message_1_responder_result srv_edhoc_respond_to_message_1(
    struct com_readonly_buffer message_1, struct edhoc_context* edhoc_context,
    struct com_writable_buffer response);

const char* srv_edhoc_message_1_responder_status_code_to_string(
    enum srv_edhoc_message_1_responder_status status);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_HANDLER_H_
