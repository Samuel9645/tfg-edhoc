/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 1 processing specific EDHOC error-response composition.
 * With a different behavior when the error is a cipher suite error
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [RFC
 * 9525 6.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-wrong-selected-cipher-suite)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_ERRORS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_ERRORS_H_

#include <edhoc.h>

#include "common/com_data_models.h"
/**
 * @brief Compose and write a Message 1 processing-specific EDHOC error message
 * into the response buffer.
 *
 * Handles both generic errors and wrong-cipher-suite errors.
 *
 * @param[in] context EDHOC context to extract the error info.
 * @param[in] error_description Generic error text for non-cipher errors.
 * @param[in] response_data Metadata of the buffer (pointer and capacity) to
 * write into
 * @return A view of the composed error
 */
struct com_readonly_buffer srv_edhoc_message_1_process_add_protocol_error(
    const struct edhoc_context* context, const char* error_description,
    struct com_writable_buffer response_data);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_ERRORS_H_
