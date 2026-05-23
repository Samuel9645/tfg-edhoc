/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 23/05/2026
 * @brief Declaration for the module to add cipher suite errors
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [RFC
 * 9528 6.3](https://datatracker.ietf.org/doc/html/rfc9528#name-wrong-selected-cipher-suite)
 */

#ifndef EDHOC_COMMON_ADD_ERROR_COM_EDHOC_ADD_CIPHER_SUITE_MISMATCH_ERROR_H_
#define EDHOC_COMMON_ADD_ERROR_COM_EDHOC_ADD_CIPHER_SUITE_MISMATCH_ERROR_H_

#include <edhoc.h>

#include "common/com_data_models.h"

/**
 * @brief Compose and write an EDHOC error message payload for a cipher suite
 * mismatch error, writing the necessary details such as the supported suites.
 * @param context Context from which to extract the cipher suites for the error
 * message
 * @param response_data Metadata of the buffer (pointer and capacity) to write
 * into com_writable_buffer
 * @return View of the buffer containing the error
 * @see [RFC
 * 9528 6.3](https://datatracker.ietf.org/doc/html/rfc9528#name-wrong-selected-cipher-suite)
 */
struct com_readonly_buffer com_edhoc_add_cipher_suite_mismatch_error(
    const struct edhoc_context* context,
    struct com_writable_buffer response_data);

#endif  // EDHOC_COMMON_ADD_ERROR_COM_EDHOC_ADD_CIPHER_SUITE_MISMATCH_ERROR_H_