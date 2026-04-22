/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Message 1 composition on client side
 * @see [RFC
 * 9528 5.2.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-initiator-composition-of-me)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_CLI_M1_COMPOSE_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_CLI_M1_COMPOSE_H_

#include <edhoc.h>

#include "common/com_data_models.h"
#include "edhoc/client/handshake/message_1/cli_m1_compose_result.h"

/**
 * @brief Compose EDHOC Message 1 payload.
 *
 * @param[in] context Initialized client handshake state.
 * @param[out] compose_buffer Destination buffer descriptor for composed
 * payload.
 * @return Result wrapper containing status and output view.
 */
struct cli_edhoc_message_1_compose_result cli_edhoc_compose_message_1(
    struct edhoc_context* context, struct com_writable_buffer* compose_buffer);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_CLI_M1_COMPOSE_H_
