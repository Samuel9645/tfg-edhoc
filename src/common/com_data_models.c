/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Definitions for helpers related to data models
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "common/com_data_models.h"

bool com_readonly_buffer_is_valid(const struct com_readonly_buffer buffer) {
  return buffer.bytes != NULL && buffer.length > 0;
}

static bool writable_buffer_is_valid(const struct com_writable_buffer* buffer) {
  return buffer != NULL && buffer->bytes != NULL;
}

bool com_writable_buffer_is_writable(const struct com_writable_buffer* buffer) {
  return writable_buffer_is_valid(buffer) && buffer->capacity > 0;
}

bool com_writable_buffer_has_content(const struct com_writable_buffer* buffer) {
  return com_writable_buffer_is_writable(buffer) && buffer->length > 0;
}

struct com_readonly_conversion_result com_writable_as_readonly(
    const struct com_writable_buffer writable) {
  if (!com_writable_buffer_has_content(&writable)) {
    return (struct com_readonly_conversion_result){
        .status = COM_RDONLY_CONV_ERR_EMPTY_BUFFER,
        .buffer = {0},
    };
  }
  return (struct com_readonly_conversion_result){
      .status = COM_RDONLY_CONV_OK,
      .buffer = {.bytes = writable.bytes, .length = writable.length},
  };
}
