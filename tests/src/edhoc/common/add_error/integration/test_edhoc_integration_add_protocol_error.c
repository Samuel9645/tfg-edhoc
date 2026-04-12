/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 11/04/2026
 * @brief Testing for the happy path with the real libedhoc of functions to add
 * edhoc related errors to buffer
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <edhoc.h>
#include <edhoc_context.h>
#include <string.h>
#include <unity.h>

#include "common/com_data_models.h"
#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

enum {
  TST_EDHOC_ADD_PROTOCOL_ERROR_CAPACITY = 256,
};

static struct tst_edhoc_add_protocol_error_env {
  uint8_t error_message[TST_EDHOC_ADD_PROTOCOL_ERROR_CAPACITY];
  struct com_writable_buffer error_buffer_view;
  enum edhoc_error_code expected_error_code;
  struct edhoc_context context;
} env = {
    .error_buffer_view = {.capacity = TST_EDHOC_ADD_PROTOCOL_ERROR_CAPACITY}};

static void trigger_invalid_context_error(void) {
  const uint8_t garbage[] = {0xFF, 0x00, 0xAA};
  edhoc_message_1_process(&env.context, garbage, sizeof(garbage));
}

static enum edhoc_error_code try_to_get_error_code(void) {
  enum edhoc_error_code expected_error_code = -1;
  if (edhoc_error_get_code(&env.context, &expected_error_code) !=
      EDHOC_SUCCESS) {
    TEST_FAIL_MESSAGE("Failed to get error code from context");
  }
  return expected_error_code;
}

void reset_env(void) {
  env.context = (struct edhoc_context){0};
  if (edhoc_context_init(&env.context) != EDHOC_SUCCESS) {
    TEST_FAIL_MESSAGE("Failed to initialize EDHOC context");
  }
  memset(env.error_message, 0, sizeof(env.error_message));
  env.error_buffer_view.length = 0;
  env.error_buffer_view.bytes = env.error_message;
}

void setUp(void) {
  reset_env();
  trigger_invalid_context_error();
  env.expected_error_code = try_to_get_error_code();
}

static void assert_add_error_status_ok(
    const enum com_edhoc_add_protocol_error_to_response_status
        add_error_status) {
  TEST_ASSERT_EQUAL(COM_EDHOC_ADD_ERROR_OK, add_error_status);
  TEST_ASSERT_GREATER_THAN_size_t_MESSAGE(
      0, env.error_buffer_view.length,
      "Error buffer length should be greater than 0");
}

static void assert_add_error_status_ok_with_message(
    const enum com_edhoc_add_protocol_error_to_response_status add_error_status,
    const char* expected_error_description) {
  TEST_ASSERT_EQUAL_MESSAGE(COM_EDHOC_ADD_ERROR_OK, add_error_status,
                            expected_error_description);
  TEST_ASSERT_GREATER_THAN_size_t_MESSAGE(
      0, env.error_buffer_view.length,
      "Error buffer length should be greater than 0");
}

static void assert_encoded_error_matches(
    const char* expected_error_description,
    const enum edhoc_error_code expected_error_code) {
  enum edhoc_error_code received_code = -1;
  char decoded_error[TST_EDHOC_ADD_PROTOCOL_ERROR_CAPACITY] = {0};
  struct edhoc_error_info received_info = {
      .text_string = decoded_error, .total_entries = sizeof(decoded_error)};
  const int process_status = edhoc_message_error_process(
      env.error_buffer_view.bytes, env.error_buffer_view.length, &received_code,
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

void test_add_error_with_description_on_invalid_library_state(void) {
  const char* expected_description = "RANDOM DESCRIPTION";

  const enum com_edhoc_add_protocol_error_to_response_status add_error_status =
      com_edhoc_add_protocol_error_with_description(
          &env.context, expected_description, &env.error_buffer_view);

  assert_add_error_status_ok(add_error_status);
  assert_encoded_error_matches(expected_description, env.expected_error_code);
}

void test_add_error_with_description_creates_valid_error_with_null_parameters(
    void) {
  const struct {
    const char* description;
    const struct edhoc_context* context;
    const char* error_description;
  } test_cases[] = {{"NULL context", NULL, "Some Error"},
                    {"NULL description", &env.context, NULL},
                    {"Both NULL", NULL, NULL}};

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    reset_env();
    if (test_cases[i].context != NULL) {
      trigger_invalid_context_error();
    }

    const enum com_edhoc_add_protocol_error_to_response_status status =
        com_edhoc_add_protocol_error_with_description(
            test_cases[i].context, test_cases[i].error_description,
            &env.error_buffer_view);

    assert_add_error_status_ok_with_message(status, test_cases[i].description);
    assert_encoded_error_matches(NULL, EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
  }
}

void test_add_error_with_error_info_on_invalid_library_state(void) {
  trigger_invalid_context_error();
  const char* expected_description = "RANDOM DESCRIPTION";
  const size_t error_description_length = strlen(expected_description);
  const struct edhoc_error_info expected_error_info = {
      .text_string = (char*)expected_description,
      .total_entries = error_description_length,
      .written_entries = error_description_length};

  const enum com_edhoc_add_protocol_error_to_response_status add_error_status =
      com_edhoc_add_protocol_error(&env.context, &expected_error_info,
                                   &env.error_buffer_view);

  assert_add_error_status_ok(add_error_status);
  assert_encoded_error_matches(expected_description, env.expected_error_code);
}

void test_add_error_with_error_info_creates_valid_error_with_null_parameters(
    void) {
  const char* expected_description = "RANDOM DESCRIPTION";
  const size_t error_description_length = strlen(expected_description);
  const struct edhoc_error_info valid_error_info = {
      .text_string = (char*)expected_description,
      .total_entries = error_description_length,
      .written_entries = error_description_length};

  const struct edhoc_error_info no_total_info = {
      .text_string = valid_error_info.text_string,
      .total_entries = 0,
      .written_entries = valid_error_info.written_entries};
  const struct edhoc_error_info no_text_info = {
      .text_string = NULL,
      .total_entries = valid_error_info.total_entries,
      .written_entries = valid_error_info.written_entries};
  const struct edhoc_error_info no_written_info = {
      .text_string = valid_error_info.text_string,
      .total_entries = valid_error_info.total_entries,
      .written_entries = 0};

  const struct {
    const char* description;
    const struct edhoc_context* context;
    const struct edhoc_error_info* info;
  } test_cases[] = {
      {"NULL context", NULL, &valid_error_info},
      {"NULL info", &env.context, NULL},
      {"Both NULL", NULL, NULL},
      {"Info: Total Entries 0", &env.context, &no_total_info},
      {"Info: Text String NULL", &env.context, &no_text_info},
      {"Info: Written Entries 0", &env.context, &no_written_info}};

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    reset_env();
    if (test_cases[i].context != NULL) {
      trigger_invalid_context_error();
    }

    const enum com_edhoc_add_protocol_error_to_response_status status =
        com_edhoc_add_protocol_error(test_cases[i].context, test_cases[i].info, &env.error_buffer_view);

    assert_add_error_status_ok_with_message(status, test_cases[i].description);
    assert_encoded_error_matches(NULL, EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
  }
}
