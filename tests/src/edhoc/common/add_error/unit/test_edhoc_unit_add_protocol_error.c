/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 11/04/2026
 * @brief Testing with mocked libedhoc of the functions to add protocol errors
 * to buffer
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <string.h>
#include <unity.h>

#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"
#include "edhoc/common/add_error/mocks/tst_mock_edhoc_error_compose.h"
#include "edhoc/common/add_error/mocks/tst_mock_edhoc_get_code.h"

enum { TST_UNIT_PROTOCOL_ERROR_BUFFER_SIZE = 128 };

static const char DESCRIPTION[] = "RANDOM DESCRIPTION";
// -1 to remove the null terminator
static const size_t DESCRIPTION_LENGTH = sizeof(DESCRIPTION) - 1;
static const struct edhoc_error_info VALID_INFO = {
    .text_string = (char*)DESCRIPTION,
    .total_entries = DESCRIPTION_LENGTH,
    .written_entries = DESCRIPTION_LENGTH,
};
static const uint8_t zeros[TST_UNIT_PROTOCOL_ERROR_BUFFER_SIZE] = {0};

static struct {
  uint8_t error_buffer[TST_UNIT_PROTOCOL_ERROR_BUFFER_SIZE];
  struct com_writable_buffer error_buffer_view;
  const struct edhoc_context dummy_context;
  const struct edhoc_error_info* valid_info;
} env = {.error_buffer_view = {.capacity = TST_UNIT_PROTOCOL_ERROR_BUFFER_SIZE},
         .valid_info = &VALID_INFO};

void setUp(void) {
  srv_edhoc_reset_error_compose_mock();
  srv_edhoc_reset_get_code_mock();
  memset(env.error_buffer, 0, sizeof(env.error_buffer));
  env.error_buffer_view.bytes = env.error_buffer;
  env.error_buffer_view.length = 0;
}

static void assert_encoded_error_matches(
    const char* expected_error_description) {
  enum edhoc_error_code received_code = -1;
  char decoded_error[TST_UNIT_PROTOCOL_ERROR_BUFFER_SIZE] = {0};
  struct edhoc_error_info received_info = {
      .text_string = decoded_error, .total_entries = sizeof(decoded_error)};
  const int process_status = edhoc_message_error_process(
      env.error_buffer_view.bytes, env.error_buffer_view.length, &received_code,
      &received_info);
  TEST_ASSERT_EQUAL_INT_MESSAGE(EDHOC_SUCCESS, process_status,
                                "CBOR Decode Failed");
  TEST_ASSERT_EQUAL(EDHOC_ERROR_CODE_UNSPECIFIED_ERROR, received_code);
  if (expected_error_description != NULL) {
    TEST_ASSERT_EQUAL_STRING(expected_error_description,
                             received_info.text_string);
  }
  TEST_ASSERT_GREATER_THAN_size_t_MESSAGE(
      0, received_info.written_entries,
      "Error buffer length should be greater than 0");
}

void test_add_protocol_error_recovers_get_code_fail(void) {
  srv_edhoc_set_get_code_failed();
  srv_edhoc_use_real_compose();

  const enum com_edhoc_add_protocol_error_to_response_status status =
      com_edhoc_add_edhoc_error_to_response(&env.dummy_context, env.valid_info,
                                            &env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_ADD_ERROR_OK, status);
  assert_encoded_error_matches(DESCRIPTION);
}

void test_add_protocol_error_fails_on_invalid_buffer(void) {
  struct com_writable_buffer invalid_buffer = {.bytes = NULL, .capacity = 0};

  const enum com_edhoc_add_protocol_error_to_response_status status =
      com_edhoc_add_edhoc_error_to_response(&env.dummy_context, env.valid_info,
                                            &invalid_buffer);

  TEST_ASSERT_EQUAL(COM_EDHOC_ADD_ERROR_ERR_INVALID_RESPONSE_BUFFER, status);
}

static void assert_response_untouched(void) {
  TEST_ASSERT_EQUAL_size_t_MESSAGE(0, env.error_buffer_view.length,
                                   "Length should not be updated on failure");
  TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(
      zeros, env.error_buffer_view.bytes, TST_UNIT_PROTOCOL_ERROR_BUFFER_SIZE,
      "Buffer memory was modified despite failure");
}

void test_add_protocol_error_fails_on_error_compose_fail(void) {
  srv_edhoc_set_error_compose_failed();

  const enum com_edhoc_add_protocol_error_to_response_status status =
      com_edhoc_add_edhoc_error_to_response(&env.dummy_context, env.valid_info,
                                            &env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_ADD_ERROR_ERR_COMPOSE, status);
  assert_response_untouched();
}

void test_add_protocol_error_with_description_recovers_get_code_fail(void) {
  srv_edhoc_set_get_code_failed();
  srv_edhoc_use_real_compose();

  const enum com_edhoc_add_protocol_error_to_response_status status =
      com_edhoc_add_edhoc_error_to_response_with_description(
          &env.dummy_context, DESCRIPTION, &env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_ADD_ERROR_OK, status);
  assert_encoded_error_matches(DESCRIPTION);
}

void test_add_protocol_error_with_description_fails_on_invalid_buffer(void) {
  struct com_writable_buffer invalid_buffer = {.bytes = NULL, .capacity = 0};

  const enum com_edhoc_add_protocol_error_to_response_status status =
      com_edhoc_add_edhoc_error_to_response_with_description(
          &env.dummy_context, DESCRIPTION, &invalid_buffer);

  TEST_ASSERT_EQUAL(COM_EDHOC_ADD_ERROR_ERR_INVALID_RESPONSE_BUFFER, status);
}

void test_add_protocol_error_with_description_fails_on_error_compose_fail(
    void) {
  srv_edhoc_set_error_compose_failed();

  const enum com_edhoc_add_protocol_error_to_response_status status =
      com_edhoc_add_edhoc_error_to_response_with_description(
          &env.dummy_context, DESCRIPTION, &env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_ADD_ERROR_ERR_COMPOSE, status);
  assert_response_untouched();
}
