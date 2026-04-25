/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Message 2 processing on client side
 * @see [RFC
 * 9528 5.5.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-initiator-processing-of-mess)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_CLI_M4_PROCESS_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_CLI_M4_PROCESS_H_

#include <edhoc.h>

#include "common/com_data_models.h"
#include "edhoc/client/handshake/message_4/cli_m4_process_result.h"

/**
 * @brief Process EDHOC Message 4 and fill error payload on failure.
 *
 * @param[in] context Initialized EDHOC context.
 * @param[in] message_4 Message 4 payload descriptor from peer.
 * @param[in] error_buffer Metadata of the buffer (pointer and capacity) to
 * write into
 * @return Result wrapper containing status and error view on failure, empty on
 * success.
 */
struct cli_edhoc_message_4_process_result cli_edhoc_process_message_4(
    struct edhoc_context* context, struct com_readonly_buffer message_4,
    struct com_writable_buffer error_buffer);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_CLI_M4_PROCESS_H_
