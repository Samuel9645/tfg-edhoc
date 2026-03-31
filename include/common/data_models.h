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

static inline bool common_request_payload_is_valid(
    const common_request_payload_t* payload) {
  return (payload != NULL) && (payload->payload != NULL) &&
         (payload->payload_length > 0);
}

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

static inline bool common_response_buffer_is_valid(
    const common_response_buffer_t* buffer) {
  return (buffer != NULL) && (buffer->payload != NULL);
}
static inline bool common_response_buffer_is_writable(
    const common_response_buffer_t* buffer) {
  return common_response_buffer_is_valid(buffer) &&
         (buffer->payload_capacity > 0);
}

static inline bool common_response_buffer_has_content(
    const common_response_buffer_t* buffer) {
  return common_response_buffer_is_writable(buffer) &&
         (buffer->payload_length > 0);
}

#endif  // COMMON_DATA_MODELS_H_
