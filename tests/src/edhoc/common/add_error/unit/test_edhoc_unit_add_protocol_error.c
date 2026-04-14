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
#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"
#include "edhoc/common/add_error/common/tst_edhoc_add_error_capacity.h"
#include "edhoc/common/add_error/environments/tst_edhoc_add_protocol_error_env.h"
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

static struct tst_edhoc_add_protocol_error_env env = {
    .error_buffer_view = {.capacity = TST_EDHOC_ADD_ERROR_CAPACITY}};

void setUp(void) {
  tst_com_edhoc_reset_error_compose_mock();
  tst_com_edhoc_reset_get_code_mock();
  tst_edhoc_add_protocol_error_reset_env(&env);
}

static void assert_status_ok(
    const enum com_edhoc_add_error_status add_error_status) {
  tst_edhoc_assert_add_error_status_ok(add_error_status,
                                       env.error_buffer_view.length);
}

void test_add_protocol_error_recovers_get_code_fail(void) {
  tst_com_edhoc_set_get_code_failed();
  tst_com_edhoc_use_real_compose();

  const enum com_edhoc_add_error_status status = com_edhoc_add_protocol_error(
      &env.context, &VALID_INFO, &env.error_buffer_view);

  assert_status_ok(status);
  tst_edhoc_assert_encoded_error_matches(env.error_buffer_view, DESCRIPTION,
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_add_protocol_error_fails_on_invalid_buffer(void) {
  struct com_writable_buffer invalid_buffer = {.bytes = NULL, .capacity = 0};

  const enum com_edhoc_add_error_status status =
      com_edhoc_add_protocol_error(&env.context, &VALID_INFO, &invalid_buffer);

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
  tst_com_edhoc_set_error_compose_failed();

  const enum com_edhoc_add_error_status status = com_edhoc_add_protocol_error(
      &env.context, &VALID_INFO, &env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_ADD_ERROR_ERR_COMPOSE, status);
  assert_response_untouched();
}

void test_add_protocol_error_with_description_recovers_get_code_fail(void) {
  tst_com_edhoc_set_get_code_failed();
  tst_com_edhoc_use_real_compose();

  const enum com_edhoc_add_error_status status =
      com_edhoc_add_protocol_error_with_description(&env.context, DESCRIPTION,
                                                    &env.error_buffer_view);

  assert_status_ok(status);
  tst_edhoc_assert_encoded_error_matches(env.error_buffer_view, DESCRIPTION,
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_add_protocol_error_with_description_fails_on_invalid_buffer(void) {
  struct com_writable_buffer invalid_buffer = {.bytes = NULL, .capacity = 0};

  const enum com_edhoc_add_error_status status =
      com_edhoc_add_protocol_error_with_description(&env.context, DESCRIPTION,
                                                    &invalid_buffer);

  TEST_ASSERT_EQUAL(COM_EDHOC_ADD_ERROR_ERR_INVALID_RESPONSE_BUFFER, status);
}

void test_add_protocol_error_with_description_fails_on_error_compose_fail(
    void) {
  tst_com_edhoc_set_error_compose_failed();

  const enum com_edhoc_add_error_status status =
      com_edhoc_add_protocol_error_with_description(&env.context, DESCRIPTION,
                                                    &env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_ADD_ERROR_ERR_COMPOSE, status);
  assert_response_untouched();
}
