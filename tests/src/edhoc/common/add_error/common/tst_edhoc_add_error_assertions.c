/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 12/04/2026
 * @brief Custom assertions for the add edhoc error tests
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"

#include <unity.h>

#include "common/com_data_models.h"
#include "edhoc/common/add_error/environments/tst_edhoc_add_internal_error_env.h"

void tst_edhoc_assert_add_error_status_ok(
    const enum com_edhoc_add_error_status status,
    const size_t error_buffer_length) {
  TEST_ASSERT_EQUAL(COM_EDHOC_ADD_ERROR_OK, status);
  TEST_ASSERT_GREATER_THAN_size_t_MESSAGE(
      0, error_buffer_length, "Error buffer length should be greater than 0");
}

void tst_edhoc_assert_add_error_status_ok_with_message(
    const enum com_edhoc_add_error_status status,
    const size_t error_buffer_length, const char* message) {
  TEST_ASSERT_EQUAL_MESSAGE(COM_EDHOC_ADD_ERROR_OK, status, message);
  TEST_ASSERT_GREATER_THAN_size_t_MESSAGE(
      0, error_buffer_length,
      "Error error_buffer length should be greater than 0");
}

void tst_edhoc_assert_encoded_error_matches(
    const struct com_writable_buffer encoded_error_buffer,
    const char* expected_error_description,
    const enum edhoc_error_code expected_error_code) {
  enum edhoc_error_code received_code = -1;
  char decoded_error[TST_EDHOC_ADD_ERROR_CAPACITY] = {0};
  struct edhoc_error_info received_info = {
      .text_string = decoded_error, .total_entries = sizeof(decoded_error)};
  const int process_status = edhoc_message_error_process(
      encoded_error_buffer.bytes, encoded_error_buffer.length, &received_code,
      &received_info);
  TEST_ASSERT_EQUAL_INT_MESSAGE(EDHOC_SUCCESS, process_status,
                                "CBOR Decode Failed");
  TEST_ASSERT_EQUAL(expected_error_code, received_code);
  if (expected_error_description != NULL) {
    TEST_ASSERT_EQUAL_STRING(expected_error_description,
                             received_info.text_string);
  }
  TEST_ASSERT_GREATER_THAN_size_t_MESSAGE(
      0, received_info.written_entries,
      "Error buffer length should be greater than 0");
}

void tst_edhoc_assert_encoded_error_is_not_empty(
    const struct com_writable_buffer encoded_error_buffer) {
  tst_edhoc_assert_encoded_error_matches(encoded_error_buffer, NULL,
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void tst_edhoc_assert_encoded_error_is_not_empty_with_code(
    const struct com_writable_buffer encoded_error_buffer,
    const enum edhoc_error_code status) {
  tst_edhoc_assert_encoded_error_matches(encoded_error_buffer, NULL, status);
}