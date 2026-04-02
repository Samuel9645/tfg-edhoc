/**
 * @file data_models.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Definitions for helpers related to data models
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "common/data_models.h"

bool com_request_payload_is_valid(const com_request_payload_t* payload) {
  return payload != NULL && payload->buffer != NULL && payload->length > 0;
}

bool com_response_buffer_is_valid(const com_response_buffer_t* buffer) {
  return buffer != NULL && buffer->buffer != NULL;
}

bool com_response_buffer_is_writable(const com_response_buffer_t* buffer) {
  return com_response_buffer_is_valid(buffer) && buffer->capacity > 0;
}

bool com_response_buffer_has_content(const com_response_buffer_t* buffer) {
  return com_response_buffer_is_writable(buffer) && buffer->length > 0;
}
