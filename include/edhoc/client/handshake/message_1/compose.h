/**
 * @file compose.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Composition API for client handshake Message 1.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_COMPOSE_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_COMPOSE_H_

#include <stddef.h>
#include <stdint.h>

#include "common/data_models.h"
#include "edhoc/client/handshake/common/state.h"
#include "edhoc/client/handshake/message_1/result.h"

/**
 * @brief Compose EDHOC Message 1 payload.
 *
 * @param[in] state Initialized client handshake state.
 * @param[out] message_1 Destination buffer descriptor for composed payload.
 * @return Result wrapper containing status and output view.
 */
edh_cli_message_1_result_t edh_cli_compose_message_1(
    edh_cli_handshake_t* state, com_writable_buffer_t* message_1);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_1_COMPOSE_H_
