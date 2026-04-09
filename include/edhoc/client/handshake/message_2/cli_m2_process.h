/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Processing API for client handshake Message 2.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_PROCESS_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_PROCESS_H_

#include "common/com_data_models.h"
#include "edhoc/client/handshake/common/cli_state.h"
#include "edhoc/client/handshake/message_2/cli_m2_result.h"

/**
 * @brief Process EDHOC Message 2 and fill error response payload on failure.
 *
 * @param[in] state Initialized client handshake state.
 * @param[in] message_2 Message 2 payload descriptor from peer.
 * @param[out] message_2_error Destination buffer descriptor for EDHOC error
 * payload.
 * @return Result wrapper containing status and output view.
 */
struct edh_cli_message_2_process_result edh_cli_process_message_2(
    struct edh_cli_handshake* state, struct com_readonly_buffer message_2,
    struct com_writable_buffer* message_2_error);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_PROCESS_H_
