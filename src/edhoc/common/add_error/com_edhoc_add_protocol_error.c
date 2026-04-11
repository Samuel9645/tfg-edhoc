/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Shared functions to compose EDHOC error payloads into response
 * buffers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

#include <string.h>

enum com_edhoc_add_protocol_error_to_response_status
com_edhoc_add_edhoc_error_to_response(
    const struct edhoc_context* context,
    const struct edhoc_error_info* error_info,
    struct com_writable_buffer* response_data) {
  if (!com_writable_buffer_is_writable(response_data)) {
    return COM_EDHOC_ADD_ERROR_ERR_INVALID_RESPONSE_BUFFER;
  }
  enum edhoc_error_code error_code;
  if (edhoc_error_get_code(context, &error_code) != EDHOC_SUCCESS) {
    return COM_EDHOC_ADD_ERROR_ERR_GET_ERROR;
  }
  if (edhoc_message_error_compose(response_data->bytes, response_data->capacity,
                                  &response_data->length, error_code,
                                  error_info) != EDHOC_SUCCESS) {
    return COM_EDHOC_ADD_ERROR_ERR_COMPOSE;
  }
  return COM_EDHOC_ADD_ERROR_OK;
}

enum com_edhoc_add_protocol_error_to_response_status
com_edhoc_add_edhoc_error_to_response_with_description(
    const struct edhoc_context* context, const char* error_description,
    struct com_writable_buffer* response_data) {
  const size_t error_description_length = strlen(error_description);
  const struct edhoc_error_info error_info = {
      .text_string = (char*)error_description,
      .total_entries = error_description_length,
      .written_entries = error_description_length,
  };
  return com_edhoc_add_edhoc_error_to_response(context, &error_info,
                                               response_data);
}
