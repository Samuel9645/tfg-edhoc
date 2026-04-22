/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Message 2 processing on client side
 * @see [RFC
 * 9528 5.3.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-initiator-processing-of-mes)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_PROCESS_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_PROCESS_H_

#include <edhoc.h>

#include "common/com_data_models.h"
#include "edhoc/client/handshake/message_2/cli_m2_process_result.h"

/**
 * @brief Process EDHOC Message 2 and fill error response payload on failure.
 *
 * @param[in] context Initialized client handshake state.
 * @param[in] message_2 Message 2 payload descriptor from peer.
 * @param[out] error_buffer Destination buffer descriptor for EDHOC error
 * payload.
 * @return Result wrapper containing status and output view.
 */
struct cli_edhoc_message_2_process_result cli_edhoc_process_message_2(
    struct edhoc_context* context, struct com_readonly_buffer message_2,
    struct com_writable_buffer* error_buffer);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_PROCESS_H_
