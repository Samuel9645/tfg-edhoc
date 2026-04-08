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

enum edh_com_set_error_info_status edh_com_set_error_info(
    const char* error_message, struct edhoc_error_info* error_info) {
  if (error_message == NULL) {
    return EDH_COM_SET_ERROR_INFO_ERR_NULL_MESSAGE;
  }
  if (error_info == NULL) {
    return EDH_COM_SET_ERROR_INFO_ERR_NULL_ERROR_INFO;
  }
  memset(error_info, 0, sizeof(struct edhoc_error_info));
  const size_t len = strlen(error_message);
  if (len == 0) {
    return EDH_COM_SET_ERROR_INFO_ERR_EMPTY_MESSAGE;
  }

  /* WARNING: As of libedhoc v1.0 (2025-04-14), text_string is treated as
   * read-only. The (char*) cast is required by the struct definition but
   * is safe for now. If libedhoc is updated, consider strdup() or stack
   * buffering. */
  error_info->text_string = (char*)error_message;
  error_info->total_entries = len;
  error_info->written_entries = len;
  return EDH_COM_SET_ERROR_INFO_OK;
}

enum edh_com_add_edhoc_error_to_response_status
edh_com_add_edhoc_error_to_response(const int edhoc_api_result,
                                    const struct edhoc_error_info* error_info,
                                    struct com_writable_buffer* response_data) {
  if (!com_writable_buffer_is_writable(response_data)) {
    return EDH_COM_ADD_ERROR_ERR_INVALID_RESPONSE_BUFFER;
  }

  if (edhoc_message_error_compose(response_data->bytes, response_data->capacity,
                                  &response_data->length, edhoc_api_result,
                                  error_info) != EDHOC_SUCCESS) {
    return EDH_COM_ADD_ERROR_ERR_COMPOSE;
  };
  return EDH_COM_ADD_ERROR_OK;
}
