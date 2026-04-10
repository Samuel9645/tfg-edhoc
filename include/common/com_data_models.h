/**
 * @file
 *
 * @brief Data models for EDHOC handshake and CoAP exchange state.
 */

#ifndef COMMON_COM_DATA_MODELS_H_
#define COMMON_COM_DATA_MODELS_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct com_readonly_buffer {
  const uint8_t* bytes;
  const size_t length;
};

// TODO: maybe create a has_content and is valid to be more explicit
bool com_readonly_buffer_is_valid(struct com_readonly_buffer buffer);

struct com_writable_buffer {
  uint8_t* bytes;
  const size_t capacity;
  size_t length;
};

bool com_writable_buffer_is_writable(const struct com_writable_buffer* buffer);

bool com_writable_buffer_has_content(const struct com_writable_buffer* buffer);

enum com_conversion_status {
  COM_RDONLY_CONV_OK = 0,
  COM_RDONLY_CONV_ERR_EMPTY_BUFFER
};

struct com_readonly_conversion_result {
  enum com_conversion_status status;
  struct com_readonly_buffer buffer;
};

struct com_readonly_conversion_result com_writable_as_readonly(
    const struct com_writable_buffer* writable);

#endif  // COMMON_COM_DATA_MODELS_H_
