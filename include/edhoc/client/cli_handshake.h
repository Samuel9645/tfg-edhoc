/**
 * @file
 * @author Samuel Rodriguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Public facade for modular client handshake init/compose/process APIs.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_H_
#define EDHOC_CLIENT_HANDSHAKE_H_

#include "common/sys_data_models.h"
#include "edhoc/client/handshake/common/cli_state.h"
#include "edhoc/client/handshake/message_1/cli_m1_result.h"
#include "edhoc/client/handshake/message_2/cli_m2_result.h"
#include "edhoc/client/handshake/message_3/cli_m3_result.h"
#include "edhoc/client/handshake/message_4/cli_m4_result.h"

/**
 * @brief Initialize client EDHOC handshake state and protocol context.
 *
 * @param[out] state Caller-owned handshake state.
 * @param credentials Credentials for EDHOC context setup.
 * @return Initialization status code.
 *
 * @note This function does not allocate memory with calloc.
 */
enum edh_cli_init_status edh_cli_init_handshake(
    struct edh_cli_handshake* state,
    const struct edhoc_credentials* credentials);

/**
 * @brief Compose EDHOC Message 1 into the provided payload buffer.
 *
 * @param[in] state Initialized handshake state.
 * @param[out] message_1 Destination buffer descriptor for Message 1 payload.
 * @return Result wrapper containing status and output view.
 */
struct edh_cli_message_1_compose_result edh_cli_handshake_compose_message_1(
    struct edh_cli_handshake* state, struct com_writable_buffer* message_1);

/**
 * @brief Process EDHOC Message 2 and compose error payload on failure.
 *
 * @param[in] state Initialized handshake state.
 * @param[in] message_2 Message 2 payload descriptor.
 * @param[out] message_2_error Destination buffer descriptor for EDHOC error
 * payload.
 * @return Result wrapper containing status and output view.
 */
struct edh_cli_message_2_process_result edh_cli_handshake_process_message_2(
    struct edh_cli_handshake* state, struct com_readonly_buffer message_2,
    struct com_writable_buffer* message_2_error);

/**
 * @brief Compose EDHOC Message 3 and compose error payload on failure.
 *
 * @param[in] state Initialized handshake state.
 * @param[out] message_3 Destination buffer descriptor for Message 3 payload on
 * success or EDHOC error payload on failure.
 * @return Result wrapper containing status and output view.
 */
struct edh_cli_message_3_compose_result edh_cli_handshake_compose_message_3(
    struct edh_cli_handshake* state, struct com_writable_buffer* message_3);

/**
 * @brief Process EDHOC Message 4 and compose error payload on failure.
 *
 * @param[in] state Initialized handshake state.
 * @param[in] message_4 Message 4 payload descriptor.
 * @param[out] message_4_error Destination buffer descriptor for EDHOC error
 * payload.
 * @return Result wrapper containing status and output view.
 */
struct edh_cli_message_4_process_result edh_cli_handshake_process_message_4(
    struct edh_cli_handshake* state, struct com_readonly_buffer message_4,
    struct com_writable_buffer* message_4_error);

#endif  // EDHOC_CLIENT_HANDSHAKE_H_
