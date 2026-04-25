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

static struct com_edhoc_add_error_result ok(
    const struct com_readonly_buffer error_message) {
  return (struct com_edhoc_add_error_result){.status = COM_EDHOC_ADD_ERROR_OK,
                                             .buffer = error_message};
}

static struct com_edhoc_add_error_result
add_internal_error_invalid_response_buffer(void) {
  return (struct com_edhoc_add_error_result){
      .status = COM_EDHOC_ADD_ERROR_ERR_INVALID_RESPONSE_BUFFER};
}

static struct com_edhoc_add_error_result internal_error(
    const enum com_edhoc_add_error_status status) {
  return (struct com_edhoc_add_error_result){.status = status};
}

struct com_edhoc_add_error_result com_edhoc_add_internal_error_result(
    const char* error_description,
    const struct com_writable_buffer response_data) {
  if (!com_writable_buffer_is_writable(response_data)) {
    return add_internal_error_invalid_response_buffer();
  }

  if (error_description == NULL) {
    error_description = "Fatal Internal Failure: error description was NULL";
  }
  struct edhoc_error_info error_info = {0};
  com_edhoc_set_error_info(&error_info, error_description);
  size_t written_length = 0;
  if (edhoc_message_error_compose(
          response_data.bytes, response_data.capacity, &written_length,
          EDHOC_ERROR_CODE_UNSPECIFIED_ERROR, &error_info) != EDHOC_SUCCESS) {
    return internal_error(COM_EDHOC_ADD_ERROR_ERR_COMPOSE);
  }
  const struct com_readonly_conversion_result conversion_result =
      com_writable_as_readonly(response_data, written_length);
  if (conversion_result.status != COM_RDONLY_CONV_OK) {
    // SHOULD NEVER HAPPEN
    return internal_error(COM_EDHOC_ADD_ERROR_ERR_CONVERSION);
  }
  return ok(conversion_result.buffer);
}

struct com_readonly_buffer com_edhoc_add_internal_error_view(
    const char* error_description,
    const struct com_writable_buffer response_data) {
  return com_edhoc_add_internal_error_result(error_description, response_data)
      .buffer;
}