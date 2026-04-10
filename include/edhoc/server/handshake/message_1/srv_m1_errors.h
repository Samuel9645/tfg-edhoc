/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 1 specific EDHOC error-response composition.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_ERRORS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_ERRORS_H_

#include <edhoc.h>

#include "common/com_data_models.h"
/**
 * @brief Compose and write a Message 1 operation-specific EDHOC error message
 * into the response buffer.
 *
 * Handles both generic errors and wrong-cipher-suite errors.
 *
 * @param[in] context EDHOC context to extract the error info.
 * @param[in] generic_error_message Generic error text for non-cipher errors.
 * @param[out] response_data Buffer where the EDHOC error message is written.
 */
void srv_edhoc_message_1_handler_add_error(
    const struct edhoc_context* context, const char* generic_error_message,
    struct com_writable_buffer* response_data);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_ERRORS_H_
