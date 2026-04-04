/**
 * @file data_models.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Definitions for helpers related to data models
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "common/data_models.h"

bool com_readonly_buffer_is_valid(const com_readonly_buffer_t buffer) {
  return buffer.bytes != NULL && buffer.length > 0;
}

static bool writable_buffer_is_valid(const com_writable_buffer_t* buffer) {
  return buffer != NULL && buffer->bytes != NULL;
}

bool com_writable_buffer_is_writable(const com_writable_buffer_t* buffer) {
  return writable_buffer_is_valid(buffer) && buffer->capacity > 0;
}

bool com_writable_buffer_has_content(const com_writable_buffer_t* buffer) {
  return com_writable_buffer_is_writable(buffer) && buffer->length > 0;
}
