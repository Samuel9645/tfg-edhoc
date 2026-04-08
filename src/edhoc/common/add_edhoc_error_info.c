/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Shared functions to compose EDHOC error payloads into response
 * buffers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/common/add_edhoc_error_info.h"

#include <string.h>

const int INTERNAL_FAILURE_EDHOC_CODE = EDHOC_ERROR_GENERIC_ERROR;

enum edh_com_add_edhoc_error_to_response_status
edh_com_add_edhoc_error_to_response(const struct edhoc_context* context,
                                    const struct edhoc_error_info* error_info,
                                    struct com_writable_buffer* response_data) {
  if (!com_writable_buffer_is_writable(response_data)) {
    return EDH_COM_ADD_ERROR_ERR_INVALID_RESPONSE_BUFFER;
  }
  enum edhoc_error_code error_code;
  if (edhoc_error_get_code(context, &error_code) != EDHOC_SUCCESS) {
    return EDH_COM_ADD_ERROR_ERR_GET_ERROR;
  }
  if (edhoc_message_error_compose(response_data->bytes, response_data->capacity,
                                  &response_data->length, error_code,
                                  error_info) != EDHOC_SUCCESS) {
    return EDH_COM_ADD_ERROR_ERR_COMPOSE;
  }
  return EDH_COM_ADD_ERROR_OK;
}

enum edh_com_add_internal_error_to_response_status
edh_com_add_internal_error_to_response(
    const char* error_message, struct com_writable_buffer* response_data) {
  if (!com_writable_buffer_is_writable(response_data)) {
    return EDH_COM_ADD_INTERNAL_ERROR_ERR_INVALID_RESPONSE_BUFFER;
  }

  const struct edhoc_error_info error_info = {
      .text_string = (char*)error_message,
      .total_entries = strlen(error_message),
  };
  if (edhoc_message_error_compose(
          response_data->bytes, response_data->capacity, &response_data->length,
          INTERNAL_FAILURE_EDHOC_CODE, &error_info) != EDHOC_SUCCESS) {
    return EDH_COM_ADD_INTERNAL_ERROR_ERR_COMPOSE;
  }
  return EDH_COM_ADD_INTERNAL_ERROR_OK;
}

enum edh_com_add_edhoc_error_to_response_status
edh_com_add_edhoc_error_to_response_with_description(
    const struct edhoc_context* context, const char* error_description,
    struct com_writable_buffer* response_data) {
  const struct edhoc_error_info error_info = {
      .text_string = (char*)error_description,
      .total_entries = strlen(error_description),
  };
  return edh_com_add_edhoc_error_to_response(context, &error_info,
                                             response_data);
}