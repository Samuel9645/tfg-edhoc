/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Shared EDHOC error-response composition helpers for server handshake
 * handlers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_COMMON_EDHOC_ERROR_H_
#define EDHOC_COMMON_EDHOC_ERROR_H_

#include <edhoc.h>

#include "common/com_data_models.h"

enum com_edhoc_add_edhoc_error_to_response_status {
  COM_EDHOC_ADD_ERROR_OK = 0,
  COM_EDHOC_ADD_ERROR_ERR_INVALID_RESPONSE_BUFFER,
  COM_EDHOC_ADD_ERROR_ERR_GET_ERROR,
  COM_EDHOC_ADD_ERROR_ERR_COMPOSE,
};

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
enum com_edhoc_add_edhoc_error_to_response_status
com_edhoc_add_edhoc_error_to_response(
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
enum com_edhoc_add_edhoc_error_to_response_status
com_edhoc_add_edhoc_error_to_response_with_description(
    const struct edhoc_context* context, const char* error_description,
    struct com_writable_buffer* response_data);

enum com_edhoc_add_internal_error_to_response_status {
  COM_EDHOC_ADD_INTERNAL_ERROR_OK = 0,
  COM_EDHOC_ADD_INTERNAL_ERROR_ERR_INVALID_RESPONSE_BUFFER,
  COM_EDHOC_ADD_INTERNAL_ERROR_ERR_COMPOSE,
};
/**
 * @brief Compose and write an EDHOC error message payload into the response
 * buffer for internal errors that are not directly related to a specific
 * libedhoc error code.
 * @param error_message Description of the error
 * @param response_data Buffer where the error message is written.
 * @return Status code indicating success or failure of the operation.
 */
enum com_edhoc_add_internal_error_to_response_status
com_edhoc_add_internal_error_to_response(
    const char* error_message, struct com_writable_buffer* response_data);

#endif  // EDHOC_COMMON_EDHOC_ERROR_H_
