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
 * @brief Generic request payload descriptor.
 */
typedef struct {
  /** Pointer to request bytes to send or process. */
  const uint8_t* payload;

  /** Length of payload in bytes. */
  size_t payload_length;
} common_request_payload_t;

bool common_request_payload_is_valid(const common_request_payload_t* payload);

/**
 * @brief Generic response buffer descriptor.
 */
typedef struct {
  /** Caller-provided buffer where response bytes are written. */
  uint8_t* payload;

  /** Capacity of payload buffer in bytes. */
  size_t payload_capacity;

  /** Output number of bytes written to payload buffer. */
  size_t payload_length;
} common_response_buffer_t;

bool common_response_buffer_is_valid(const common_response_buffer_t* buffer);
bool common_response_buffer_is_writable(const common_response_buffer_t* buffer);
bool common_response_buffer_has_content(const common_response_buffer_t* buffer);

#endif  // COMMON_DATA_MODELS_H_
