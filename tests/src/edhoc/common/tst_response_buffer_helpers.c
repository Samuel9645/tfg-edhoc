/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 08/04/2026
 * @brief Helpers for common operations on response buffers in tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "edhoc/common/tst_response_buffer_helpers.h"

#include "common/tst_report_mock_error.h"
#include "unity.h"

bool com_edhoc_response_buffer_is_null(
    const struct com_writable_buffer* buffer) {
  if (buffer == NULL) {
    tst_report_mock_error("response buffer is NULL");
    return true;
  }
  if (buffer->bytes == NULL) {
    tst_report_mock_error("response buffer bytes is NULL");
    return true;
  }
  return false;
}