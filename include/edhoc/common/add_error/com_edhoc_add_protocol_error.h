/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Shared EDHOC error-response composition helpers for server handshake
 * handlers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_COMMON_ADD_ERROR_COM_EDHOC_ADD_PROTOCOL_ERROR_H_
#define EDHOC_COMMON_ADD_ERROR_COM_EDHOC_ADD_PROTOCOL_ERROR_H_

#include <edhoc.h>

#include "common/com_data_models.h"
#include "edhoc/common/add_error/com_edhoc_add_error_status.h"

/**
 * @brief Compose and write an EDHOC error message payload into the response
 * buffer.
 *
 * Caller is responsible for building error_info according to the operation
 * context (for example Message 1 process vs Message 2 compose) and passing it
 * to this function.
 *
 * @param[in] context Context used to translate libedhoc error to protocol
 * error.
 * @param[in] error_info Prepared EDHOC error info to embed in the response.
 * @param[out] response_data Buffer where the EDHOC error message is written.
 * @return Status code indicating success or failure of the operation.
 */
struct com_edhoc_add_error_result com_edhoc_add_protocol_error_result(
    const struct edhoc_context* context,
    const struct edhoc_error_info* error_info,
    struct com_writable_buffer* response_data);

/**
 * @brief Convenience wrapper for com_edhoc_add_protocol_error_result().
 * @param[in] context Context used to translate libedhoc error to protocol
 * error.
 * @param[in] error_info Prepared EDHOC error info to embed in the response.
 * @param[out] response_data Buffer where the EDHOC error message is written.
 * @return A readonly buffer view containing the composed error message.
 * @note This function is intended for protocol logic where the diagnostic
 * status code is not required and only the resulting payload is needed.
 */
struct com_readonly_buffer com_edhoc_add_protocol_error_view(
    const struct edhoc_context* context,
    const struct edhoc_error_info* error_info,
    struct com_writable_buffer* response_data);

/**
 * @brief Compose and write an EDHOC error message payload into the response
 * buffer.
 *
 * Caller is responsible for building error_info according to the operation
 * context (for example Message 1 process vs Message 2 compose) and passing it
 * to this function.
 *
 * @param[in] context Context used to translate libedhoc error to protocol
 * error.
 * @param[in] error_description Description of the error to include in the
 * error_info text_string
 * @param[out] response_data Buffer where the EDHOC error message is written.
 * @return Status code indicating success or failure of the operation.
 */
struct com_edhoc_add_error_result
com_edhoc_add_protocol_error_with_description_result(
    const struct edhoc_context* context, const char* error_description,
    struct com_writable_buffer* response_data);

/**
 * @brief Convenience wrapper for
 * com_edhoc_add_protocol_error_with_description_result().
 * @param[in] context Context used to translate libedhoc error to protocol
 * error.
 * @param[in] error_description Description of the error to include in the
 * error_info text_string
 * @param[out] response_data Buffer where the EDHOC error message is written.
 * @return A readonly buffer view containing the composed error message.
 * @note This function is intended for protocol logic where the diagnostic
 * status code is not required and only the resulting payload is needed.
 */
struct com_readonly_buffer com_edhoc_add_protocol_error_with_description_view(
    const struct edhoc_context* context, const char* error_description,
    struct com_writable_buffer* response_data);

#endif  // EDHOC_COMMON_ADD_ERROR_COM_EDHOC_ADD_PROTOCOL_ERROR_H_
