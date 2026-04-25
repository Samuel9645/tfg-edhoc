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
#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"
#include "edhoc/common/add_error/environments/tst_edhoc_add_protocol_error_env.h"

static struct tst_edhoc_add_protocol_error_env env = {
    .error_buffer_view = {.capacity = TST_EDHOC_ADD_ERROR_CAPACITY}};

static enum edhoc_error_code expected_error_code = -1;

static void try_to_get_error_code(void) {
  expected_error_code = -1;
  if (edhoc_error_get_code(&env.context, &expected_error_code) !=
      EDHOC_SUCCESS) {
    TEST_FAIL_MESSAGE("Failed to get error code from context");
  }
}

static void trigger_library_error(struct edhoc_context* context) {
  if (edhoc_context_init(context) != EDHOC_SUCCESS) {
    TEST_FAIL_MESSAGE("Failed to initialize EDHOC context");
  }
  const uint8_t garbage[] = {0xFF, 0x00, 0xAA};
  edhoc_message_1_process(context, garbage, sizeof(garbage));
}

static void reset_test_state(
    struct tst_edhoc_add_protocol_error_env* environment) {
  tst_edhoc_add_protocol_error_reset_env(environment);
  trigger_library_error(&environment->context);
}

void setUp(void) {
  reset_test_state(&env);
  try_to_get_error_code();
}

void assert_result_matches(const char* expected_description,
                           const struct com_edhoc_add_error_result result) {
  tst_edhoc_assert_add_error_status_ok(result);
  tst_edhoc_assert_encoded_error_matches(result.buffer, expected_description,
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_add_error_with_description_on_invalid_library_state(void) {
  const char* expected_description = "RANDOM DESCRIPTION";

  const struct com_edhoc_add_error_result result =
      com_edhoc_add_protocol_error_with_description_result(
          &env.context, expected_description, env.error_buffer_view);

  assert_result_matches(expected_description, result);
}

static void reset_test_case_state(struct edhoc_context* context) {
  tst_edhoc_add_protocol_error_reset_env(&env);
  if (context != NULL) {
    trigger_library_error(context);
  }
}

void test_add_error_with_description_creates_valid_error_with_null_parameters(
    void) {
  const struct {
    const char* description;
    struct edhoc_context* context;
    const char* error_description;
  } test_cases[] = {{"NULL context", NULL, "Some Error"},
                    {"NULL description", &env.context, NULL},
                    {"Both NULL", NULL, NULL}};

  const size_t test_length = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < test_length; i++) {
    const struct com_edhoc_add_error_result result =
        com_edhoc_add_protocol_error_with_description_result(
            test_cases[i].context, test_cases[i].error_description,
            env.error_buffer_view);

    tst_edhoc_assert_add_error_status_ok_with_message(
        result, test_cases[i].description);
    tst_edhoc_assert_encoded_error_is_not_empty(result.buffer);

    if (i < test_length - 1) {
      reset_test_case_state(test_cases[i + 1].context);
    }
  }
}

void test_add_error_with_error_info_on_invalid_library_state(void) {
  const char* expected_description = "RANDOM DESCRIPTION";
  const size_t error_description_length = strlen(expected_description);
  const struct edhoc_error_info expected_error_info = {
      .text_string = (char*)expected_description,
      .total_entries = error_description_length,
      .written_entries = error_description_length};

  const struct com_edhoc_add_error_result result =
      com_edhoc_add_protocol_error_result(&env.context, &expected_error_info,
                                          env.error_buffer_view);

  assert_result_matches(expected_description, result);
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
    struct edhoc_context* context;
    const struct edhoc_error_info* info;
  } test_cases[] = {
      {"NULL context", NULL, &valid_error_info},
      {"NULL info", &env.context, NULL},
      {"Both NULL", NULL, NULL},
      {"Info: Total Entries 0", &env.context, &no_total_info},
      {"Info: Text String NULL", &env.context, &no_text_info},
      {"Info: Written Entries 0", &env.context, &no_written_info}};

  const size_t test_length = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < test_length; i++) {
    const struct com_edhoc_add_error_result result =
        com_edhoc_add_protocol_error_result(
            test_cases[i].context, test_cases[i].info, env.error_buffer_view);

    tst_edhoc_assert_add_error_status_ok_with_message(
        result, test_cases[i].description);
    tst_edhoc_assert_encoded_error_is_not_empty(result.buffer);

    if (i < test_length - 1) {
      reset_test_case_state(test_cases[i + 1].context);
    }
  }
}
