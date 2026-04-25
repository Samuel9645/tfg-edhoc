/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Shared EDHOC error-response composition helpers for server handshake
 * handlers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_COMMON_ADD_ERROR_COM_EDHOC_ADD_INTERNAL_ERROR_H_
#define EDHOC_COMMON_ADD_ERROR_COM_EDHOC_ADD_INTERNAL_ERROR_H_

#include "common/com_data_models.h"
#include "edhoc/common/add_error/com_edhoc_add_error_status.h"

/**
 * @brief Compose and write an EDHOC error message payload into the response
 * buffer for internal errors that are not directly related to a specific
 * libedhoc error code.
 * @param[in] error_description Description of the error
 * @param[in] response_data Metadata of the buffer (pointer and capacity) to
 * write into com_writable_buffer
 * @return Status code indicating success or failure of the operation.
 */
struct com_edhoc_add_error_result com_edhoc_add_internal_error_result(
    const char* error_description, struct com_writable_buffer response_data);
/**
 * @brief Convenience wrapper for com_edhoc_add_internal_error_result().
 * @param[in] error_description Description of the error.
 * @param[in] response_data Metadata of the buffer (pointer and capacity) to
 * write into com_writable_buffer
 * @return A readonly buffer view containing the composed error message.
 * @note This function is intended for protocol logic where the diagnostic
 * status code is not required and only the resulting payload is needed.
 */
struct com_readonly_buffer com_edhoc_add_internal_error_view(
    const char* error_description, struct com_writable_buffer response_data);

#endif  // EDHOC_COMMON_ADD_ERROR_COM_EDHOC_ADD_INTERNAL_ERROR_H_