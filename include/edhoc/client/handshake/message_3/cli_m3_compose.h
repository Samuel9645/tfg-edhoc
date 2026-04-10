/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Composition API for client handshake Message 3.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_CLI_M3_COMPOSE_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_CLI_M3_COMPOSE_H_

#include "common/com_data_models.h"
#include "edhoc/client/handshake/common/cli_state.h"
#include "edhoc/client/handshake/message_3/cli_m3_result.h"

/**
 * @brief Compose EDHOC Message 3 and fill error payload on failure.
 *
 * @param[in] state Initialized client handshake state.
 * @param[out] message_3_or_error Destination buffer descriptor for Message 3 or
 * EDHOC error payload.
 * @return Result wrapper containing status and output view.
 */
struct cli_edhoc_message_3_compose_result cli_edhoc_compose_message_3(
    struct cli_edhoc_handshake* state,
    struct com_writable_buffer* message_3_or_error);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_CLI_M3_COMPOSE_H_
