/**
 * @file
 * @author Samuel Rodriguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Public facade for modular client handshake init/compose/process APIs.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_H_
#define EDHOC_CLIENT_HANDSHAKE_H_

#include "common/data_models.h"
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
edh_cli_init_status_t edh_cli_init_handshake(
    edh_cli_handshake_t* state, const struct edhoc_credentials* credentials);

/**
 * @brief Compose EDHOC Message 1 into the provided payload buffer.
 *
 * @param[in] state Initialized handshake state.
 * @param[out] message_1 Destination buffer descriptor for Message 1 payload.
 * @return Result wrapper containing status and output view.
 */
edh_cli_message_1_result_t edh_cli_handshake_compose_message_1(
    edh_cli_handshake_t* state, com_writable_buffer_t* message_1);

/**
 * @brief Process EDHOC Message 2 and compose error payload on failure.
 *
 * @param[in] state Initialized handshake state.
 * @param[in] message_2 Message 2 payload descriptor.
 * @param[out] message_2_error Destination buffer descriptor for EDHOC error
 * payload.
 * @return Result wrapper containing status and output view.
 */
edh_cli_message_2_result_t edh_cli_handshake_process_message_2(
    edh_cli_handshake_t* state, com_readonly_buffer_t message_2,
    com_writable_buffer_t* message_2_error);

/**
 * @brief Compose EDHOC Message 3 and compose error payload on failure.
 *
 * @param[in] state Initialized handshake state.
 * @param[out] message_3 Destination buffer descriptor for Message 3 payload on
 * success or EDHOC error payload on failure.
 * @return Result wrapper containing status and output view.
 */
edh_cli_message_3_result_t edh_cli_handshake_compose_message_3(
    edh_cli_handshake_t* state, com_writable_buffer_t* message_3);

/**
 * @brief Process EDHOC Message 4 and compose error payload on failure.
 *
 * @param[in] state Initialized handshake state.
 * @param[in] message_4 Message 4 payload descriptor.
 * @param[out] message_4_error Destination buffer descriptor for EDHOC error
 * payload.
 * @return Result wrapper containing status and output view.
 */
edh_cli_message_4_result_t edh_cli_handshake_process_message_4(
    edh_cli_handshake_t* state, com_readonly_buffer_t message_4,
    com_writable_buffer_t* message_4_error);

#endif  // EDHOC_CLIENT_HANDSHAKE_H_
