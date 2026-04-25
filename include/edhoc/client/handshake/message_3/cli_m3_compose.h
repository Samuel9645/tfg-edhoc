/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Message 3 composition on client side
 * @see [RFC
 * 9528 5.4.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-initiator-composition-of-mes
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_CLI_M3_COMPOSE_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_CLI_M3_COMPOSE_H_

#include <edhoc.h>

#include "common/com_data_models.h"
#include "edhoc/client/handshake/message_3/cli_m3_compose_result.h"

/**
 * @brief Compose EDHOC Message 3 and fill error payload on failure.
 *
 * @param[in] context Initialized EDHOC context.
 * @param[in] compose_buffer Metadata of the buffer (pointer and capacity) to
 * write into
 * @return Result wrapper containing status and output view.
 */
struct cli_edhoc_message_3_compose_result cli_edhoc_compose_message_3(
    struct edhoc_context* context, struct com_writable_buffer compose_buffer);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_3_CLI_M3_COMPOSE_H_
