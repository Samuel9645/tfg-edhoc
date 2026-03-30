/**
 * @file data_models.h
 *
 * @brief Data models for EDHOC handshake and CoAP exchange state.
 */

#ifndef COMMON_DATA_MODELS_H_
#define COMMON_DATA_MODELS_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Generic request payload descriptor.
 */
typedef struct {
  /** Pointer to request bytes to send or process. */
  const uint8_t* payload;

  /** Length of payload in bytes. */
  size_t payload_len;
} common_request_payload_t;

/**
 * @brief Generic response buffer descriptor.
 */
typedef struct {
  /** Caller-provided buffer where response bytes are written. */
  uint8_t* payload;

  /** Capacity of payload buffer in bytes. */
  size_t payload_capacity;

  /** Output number of bytes written to payload buffer. */
  size_t* payload_len;
} common_response_buffer_t;

#endif  // COMMON_DATA_MODELS_H_
