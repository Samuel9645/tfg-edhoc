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

enum com_edhoc_add_internal_error_to_response_status {
  COM_EDHOC_ADD_INTERNAL_ERROR_OK = 0,
  COM_EDHOC_ADD_INTERNAL_ERROR_ERR_INVALID_RESPONSE_BUFFER,
  COM_EDHOC_ADD_INTERNAL_ERROR_ERR_COMPOSE,
};
/**
 * @brief Compose and write an EDHOC error message payload into the response
 * buffer for internal errors that are not directly related to a specific
 * libedhoc error code.
 * @param[in] error_message Description of the error
 * @param[out] response_data Buffer where the error message is written.
 * @return Status code indicating success or failure of the operation.
 */
enum com_edhoc_add_internal_error_to_response_status
com_edhoc_add_internal_error_to_response(
    const char* error_message, struct com_writable_buffer* response_data);

#endif  // EDHOC_COMMON_ADD_ERROR_COM_EDHOC_ADD_INTERNAL_ERROR_H_