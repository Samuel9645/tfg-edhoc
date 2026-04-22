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
 * @param[in] context Initialized client handshake state.
 * @param[out] message_2_error Destination buffer descriptor for EDHOC error
 * payload.
 * @return Result wrapper containing status and output view.
 */
struct cli_edhoc_message_2_process_result cli_edhoc_process_message_2(
    struct edhoc_context* context, struct com_writable_buffer* error_buffer);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_PROCESS_H_
