/**
 * @file compose.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Composition API for client handshake Message 3.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_COMPOSE_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_COMPOSE_H_

#include <stddef.h>
#include <stdint.h>

#include "common/data_models.h"
#include "edhoc/client/handshake/common/state.h"
#include "edhoc/client/handshake/message_3/result.h"

/**
 * @brief Compose EDHOC Message 3 and fill error payload on failure.
 *
 * @param[in] state Initialized client handshake state.
 * @param[out] message_3 Destination buffer descriptor for Message 3 or EDHOC
 * error payload.
 * @return Result wrapper containing status and output view.
 */
edh_cli_message_3_result_t edh_cli_compose_message_3(
    edh_cli_handshake_t* state, com_writable_buffer_t* message_3);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_COMPOSE_H_
