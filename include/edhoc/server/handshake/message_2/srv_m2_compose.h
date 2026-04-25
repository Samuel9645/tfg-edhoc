/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 2 composition on the responder side
 * @see [RFC
 * 9528 5.3.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-composition-of-me)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_2_SRV_M2_COMPOSE_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_2_SRV_M2_COMPOSE_H_

#include <edhoc.h>

#include "edhoc/server/handshake/message_2/srv_m2_compose_result.h"

struct srv_edhoc_message_2_compose_result srv_edhoc_compose_message_2(
    struct edhoc_context* context, struct com_writable_buffer compose_buffer);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_2_SRV_M2_COMPOSE_H_