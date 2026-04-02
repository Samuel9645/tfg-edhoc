/**
 * @file errors.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 1 specific EDHOC error-response composition.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_ERRORS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_ERRORS_H_

#include <edhoc.h>

#include "common/data_models.h"

/**
 * @brief Compose and write a Message 1 operation-specific EDHOC error message
 * into the response buffer.
 *
 * Handles both generic errors and wrong-cipher-suite errors with appropriate
 * error info context.
 *
 * @param[in] edhoc_api_result Return value from the failing libedhoc API call.
 * @param[in] edhoc_context EDHOC context carrying operation failure details.
 * @param[in] generic_error_message Generic error text for non-cipher errors.
 * @param[out] response_data Buffer where the EDHOC error message is written.
 */
void tst_edh_message_1_handler_add_error(
    int edhoc_api_result, const struct edhoc_context* edhoc_context,
                                     const char* generic_error_message,
                                     com_response_buffer_t* response_data);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_ERRORS_H_
