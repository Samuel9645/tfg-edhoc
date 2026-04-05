/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Message 4 specific EDHOC error-response composition for client side.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_ERRORS_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_ERRORS_H_

#include "common/data_models.h"

/**
 * @brief Compose EDHOC error payload for Message 4 operation failures.
 *
 * @param[in] edhoc_api_result Failing libedhoc return code.
 * @param[in] generic_error_message Generic diagnostic message for payload text.
 * @param[out] response_data Destination buffer descriptor for EDHOC error
 * payload bytes.
 */
void edh_cli_message_4_handler_add_error(int edhoc_api_result,
                                         const char* generic_error_message,
    struct com_writable_buffer* response_data);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_ERRORS_H_
