/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Composition API for client handshake Message 1.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_COMPOSE_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_COMPOSE_H_

#include "common/sys_data_models.h"
#include "edhoc/client/handshake/common/cli_state.h"
#include "edhoc/client/handshake/message_1/cli_m1_result.h"

/**
 * @brief Compose EDHOC Message 1 payload.
 *
 * @param[in] state Initialized client handshake state.
 * @param[out] message_1 Destination buffer descriptor for composed payload.
 * @return Result wrapper containing status and output view.
 */
struct edh_cli_message_1_compose_result edh_cli_compose_message_1(
    struct edh_cli_handshake* state, struct com_writable_buffer* message_1);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_COMPOSE_H_
