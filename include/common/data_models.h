/**
 * @file data_models.h
 *
 * @brief Data models for EDHOC handshake and CoAP exchange state.
 */

#ifndef COMMON_DATA_MODELS_H_
#define COMMON_DATA_MODELS_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Generic readonly buffer
 */
typedef struct com_readonly_buffer {
  /** Pointer to the data */
  const uint8_t* bytes;

  /** Input length of data to process in bytes. */
  size_t length;
} com_readonly_buffer_t;

bool com_readonly_buffer_is_valid(const com_readonly_buffer_t* buffer);

/**
 * @brief Generic response buffer descriptor.
 */
typedef struct com_writable_buffer {
  /** Caller-provided buffer where response bytes are written. */
  uint8_t* bytes;

  /** Capacity of buffer in bytes. */
  size_t capacity;

  /** Output number of bytes written to buffer. */
  size_t length;
} com_writable_buffer_t;

bool com_writable_buffer_is_writable(const com_writable_buffer_t* buffer);

bool com_writable_buffer_has_content(const com_writable_buffer_t* buffer);

#endif  // COMMON_DATA_MODELS_H_
