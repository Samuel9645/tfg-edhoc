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

/**
 * @brief Readonly buffer used as output of operations that return a view over a
 * caller-owned memory region.
 */
struct com_readonly_buffer {
  const uint8_t* const bytes;
  const size_t length;
};

bool com_readonly_buffer_has_content(struct com_readonly_buffer buffer);

/**
 * @brief Writable buffer view used as input to write data into a caller-owned
 * memory region.
 *
 * The caller must inspect the resulting @ref com_readonly_buffer or the
 * corresponding length field in the operation result to know how many bytes
 * were actually written.
 */
struct com_writable_buffer {
  uint8_t* bytes;
  const size_t capacity;
};

/**
 * @brief Checks whether a writable buffer view can be used for writes.
 * @param[in] buffer Writable buffer view provided as input. `bytes` points to
 * the writable area and `capacity` is the maximum number of bytes available.
 * @return true when the buffer is writable and has positive capacity.
 */
bool com_writable_buffer_is_writable(struct com_writable_buffer buffer);

enum com_conversion_status {
  COM_RDONLY_CONV_OK = 0,
  COM_RDONLY_CONV_ERR_EMPTY_BUFFER
};

struct com_readonly_conversion_result {
  const enum com_conversion_status status;
  const struct com_readonly_buffer buffer;
};

/**
 * @brief Converts a writable buffer view into a readonly view using the real
 * number of bytes written by the caller.
 * @param[in] buffer Writable buffer view provided as input. `bytes` points to
 * the writable area and `capacity` is the maximum number of bytes available.
 * @param[in] written_length Number of bytes actually written into @p buffer.
 * @return Conversion result containing either the readonly view or an empty
 * view when @p written_length is zero.
 */
struct com_readonly_conversion_result com_writable_as_readonly(
    struct com_writable_buffer buffer, size_t written_length);

#endif  // COMMON_COM_DATA_MODELS_H_
