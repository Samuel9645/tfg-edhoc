/**
 * @file
 *
 * @brief Data models for EDHOC handshake and CoAP exchange state.
 */

#ifndef COMMON_DATA_MODELS_H_
#define COMMON_DATA_MODELS_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// TODO: make length const
struct com_readonly_buffer {
  const uint8_t* bytes;
  size_t length;
};

bool com_readonly_buffer_is_valid(struct com_readonly_buffer buffer);

struct com_writable_buffer {
  uint8_t* bytes;
  size_t capacity;
  size_t length;
};

bool com_writable_buffer_is_writable(const struct com_writable_buffer* buffer);

bool com_writable_buffer_has_content(const struct com_writable_buffer* buffer);

#endif  // COMMON_DATA_MODELS_H_
