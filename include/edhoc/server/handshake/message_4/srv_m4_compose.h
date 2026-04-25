/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Message 4 compose on the responder side.
 * @see [RFC
 * 9528 5.5.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-composition-of-mes)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_4_SRV_M4_COMPOSE_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_4_SRV_M4_COMPOSE_H_

#include <edhoc.h>

#include "edhoc/server/handshake/message_4/srv_m4_compose_result.h"

/**
 * @brief Compose EDHOC Message 4 into a writable buffer view.
 * @param[in] context EDHOC context used to generate the message.
 * @param[in] compose_buffer Metadata of the buffer (pointer and capacity) to
 * write into
 */
struct srv_edhoc_message_4_compose_result srv_edhoc_compose_message_4(
    struct edhoc_context* context, struct com_writable_buffer compose_buffer);

const char* srv_edhoc_compose_message_4_status_code_to_string(
    enum srv_edhoc_message_4_compose_status status);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_4_SRV_M4_COMPOSE_H_
