/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 11/04/2026
 * @brief Definitions for the com_edhoc_add_internal_error module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"

#include <edhoc.h>

#include "edhoc/common/add_error/internal/com_edhoc_set_error_info.h"

const int INTERNAL_FAILURE_EDHOC_CODE = EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;

enum com_edhoc_add_internal_error_to_response_status
com_edhoc_add_internal_error_to_response(
    const char* error_message, struct com_writable_buffer* response_data) {
  if (!com_writable_buffer_is_writable(response_data)) {
    return COM_EDHOC_ADD_INTERNAL_ERROR_ERR_INVALID_RESPONSE_BUFFER;
  }

  struct edhoc_error_info error_info = {0};
  com_edhoc_set_error_info(&error_info, error_message);
  if (edhoc_message_error_compose(
          response_data->bytes, response_data->capacity, &response_data->length,
          INTERNAL_FAILURE_EDHOC_CODE, &error_info) != EDHOC_SUCCESS) {
    return COM_EDHOC_ADD_INTERNAL_ERROR_ERR_COMPOSE;
  }
  return COM_EDHOC_ADD_INTERNAL_ERROR_OK;
}
