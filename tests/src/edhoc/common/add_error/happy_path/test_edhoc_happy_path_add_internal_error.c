/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 11/04/2026
 * @brief Testing for the happy path of functions to add internal errors to
 * buffer
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <edhoc.h>
#include <stdint.h>
#include <string.h>
#include <unity.h>

#include "common/com_data_models.h"
#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"

enum {
  TST_EDHOC_ADD_INTERNAL_ERROR_CAPACITY = 256,
};

static struct {
  uint8_t error_message[TST_EDHOC_ADD_INTERNAL_ERROR_CAPACITY];
  struct com_writable_buffer error_buffer_view;
} env = {
    .error_buffer_view = {.capacity = TST_EDHOC_ADD_INTERNAL_ERROR_CAPACITY}};

void setUp(void) {
  memset(env.error_message, 0, sizeof(env.error_message));
  env.error_buffer_view.length = 0;
  env.error_buffer_view.bytes = env.error_message;
}

static void assert_add_error_status_ok(
    const enum com_edhoc_add_internal_error_to_response_status
        add_error_status) {
  TEST_ASSERT_EQUAL(COM_EDHOC_ADD_INTERNAL_ERROR_OK, add_error_status);
  TEST_ASSERT_GREATER_THAN_size_t_MESSAGE(
      0, env.error_buffer_view.length,
      "Error buffer length should be greater than 0");
}

static void assert_encoded_error_matches(
    const char* expected_error_description,
    const enum edhoc_error_code expected_error_code) {
  enum edhoc_error_code received_code = -1;
  char decoded_error[TST_EDHOC_ADD_INTERNAL_ERROR_CAPACITY] = {0};
  struct edhoc_error_info received_info = {
      .text_string = decoded_error, .total_entries = sizeof(decoded_error)};
  const int process_status = edhoc_message_error_process(
      env.error_buffer_view.bytes, env.error_buffer_view.length, &received_code,
      &received_info);
  TEST_ASSERT_EQUAL_INT_MESSAGE(EDHOC_SUCCESS, process_status,
                                "CBOR Decode Failed");
  TEST_ASSERT_EQUAL(expected_error_code, received_code);
  TEST_ASSERT_EQUAL_STRING(expected_error_description,
                           received_info.text_string);
  TEST_ASSERT_GREATER_THAN_size_t_MESSAGE(
      0, received_info.written_entries,
      "Error buffer length should be greater than 0");
}

static void assert_error_contains_data(void) {
  enum edhoc_error_code received_code = -1;
  char decoded_error[TST_EDHOC_ADD_INTERNAL_ERROR_CAPACITY] = {0};
  struct edhoc_error_info received_info = {
      .text_string = decoded_error, .total_entries = sizeof(decoded_error)};
  const int process_status = edhoc_message_error_process(
      env.error_buffer_view.bytes, env.error_buffer_view.length, &received_code,
      &received_info);
  TEST_ASSERT_EQUAL_INT_MESSAGE(EDHOC_SUCCESS, process_status,
                                "CBOR Decode Failed");
  TEST_ASSERT_EQUAL(EDHOC_ERROR_CODE_UNSPECIFIED_ERROR, received_code);
  TEST_ASSERT_GREATER_THAN_size_t_MESSAGE(
      0, received_info.written_entries,
      "Error buffer length should be greater than 0");
}

void test_add_internal_error(void) {
  const char* expected_error_description = "RANDOM DESCRIPTION";

  const enum com_edhoc_add_internal_error_to_response_status add_error_status =
      com_edhoc_add_internal_error_to_response(expected_error_description,
                                               &env.error_buffer_view);

  assert_add_error_status_ok(add_error_status);
  assert_encoded_error_matches(expected_error_description,
                               EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_add_internal_error_creates_valid_error_on_null_message(void) {
  const enum com_edhoc_add_internal_error_to_response_status add_error_status =
      com_edhoc_add_internal_error_to_response(NULL, &env.error_buffer_view);

  assert_add_error_status_ok(add_error_status);
  assert_error_contains_data();
}
