/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Shared functions to compose EDHOC error payloads into response
 * buffers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

#include "edhoc/common/add_error/internal/com_edhoc_set_error_info.h"

static bool error_info_is_invalid(const struct edhoc_error_info* error_info) {
  return error_info == NULL || error_info->written_entries == 0 ||
         error_info->text_string == NULL || error_info->total_entries == 0;
}

static struct com_edhoc_add_error_result add_internal_error_ok(
    const struct com_readonly_buffer error_message) {
  return (struct com_edhoc_add_error_result){.status = COM_EDHOC_ADD_ERROR_OK,
                                             .buffer = error_message};
}

static struct com_edhoc_add_error_result
add_internal_error_invalid_response_buffer(void) {
  return (struct com_edhoc_add_error_result){
      .status = COM_EDHOC_ADD_ERROR_ERR_INVALID_RESPONSE_BUFFER};
}

static struct com_edhoc_add_error_result add_internal_error(
    const enum com_edhoc_add_error_status status) {
  return (struct com_edhoc_add_error_result){.status = status};
}

struct com_edhoc_add_error_result com_edhoc_add_protocol_error(
    const struct edhoc_context* context,
    const struct edhoc_error_info* error_info,
    struct com_writable_buffer* response_data) {
  if (!com_writable_buffer_is_writable(response_data)) {
    return add_internal_error_invalid_response_buffer();
  }

  struct edhoc_error_info default_error_info = {0};
  if (error_info_is_invalid(error_info)) {
    error_info = &default_error_info;
    com_edhoc_set_error_info(
        &default_error_info,
        "Fatal Internal Error: invalid error info provided");
  }
  const enum edhoc_error_code fallback_error_code =
      EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;
  enum edhoc_error_code error_code = fallback_error_code;
  if (context != NULL &&
      edhoc_error_get_code(context, &error_code) != EDHOC_SUCCESS) {
    error_code = fallback_error_code;
  }
  if (edhoc_message_error_compose(
          response_data->bytes, response_data->capacity, &response_data->length,
          EDHOC_ERROR_CODE_UNSPECIFIED_ERROR, error_info) != EDHOC_SUCCESS) {
    return add_internal_error(COM_EDHOC_ADD_ERROR_ERR_COMPOSE);
  }
  const struct com_readonly_conversion_result conversion_result =
      com_writable_as_readonly(response_data);
  if (conversion_result.status != COM_RDONLY_CONV_OK) {
    // SHOULD NEVER HAPPEN
    return add_internal_error(COM_EDHOC_ADD_ERROR_ERR_CONVERSION);
  }
  return add_internal_error_ok(conversion_result.buffer);
}

struct com_edhoc_add_error_result com_edhoc_add_protocol_error_with_description(
    const struct edhoc_context* context, const char* error_description,
    struct com_writable_buffer* response_data) {
  if (error_description == NULL) {
    error_description = "Fatal Internal Error: error description was NULL";
  }
  struct edhoc_error_info error_info = {0};
  com_edhoc_set_error_info(&error_info, error_description);
  return com_edhoc_add_protocol_error(context, &error_info, response_data);
}
