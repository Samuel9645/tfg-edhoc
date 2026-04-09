/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Processing API for client handshake Message 4.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_PROCESS_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_PROCESS_H_

#include "common/sys_data_models.h"
#include "edhoc/client/handshake/common/cli_state.h"
#include "edhoc/client/handshake/message_4/cli_m4_result.h"

/**
 * @brief Process EDHOC Message 4 and fill error payload on failure.
 *
 * @param[in] state Initialized client handshake state.
 * @param[in] message_4 Message 4 payload descriptor from peer.
 * @param[out] message_4_error Destination buffer descriptor for EDHOC error
 * payload.
 * @return Result wrapper containing status and output view.
 */
struct edh_cli_message_4_process_result edh_cli_process_message_4(
    struct edh_cli_handshake* state, struct com_readonly_buffer message_4,
    struct com_writable_buffer* message_4_error);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_PROCESS_H_
