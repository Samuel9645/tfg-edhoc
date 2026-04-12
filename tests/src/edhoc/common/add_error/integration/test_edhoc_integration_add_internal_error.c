/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 11/04/2026
 * @brief Testing for the happy path with the real libedhoc of functions to add
 * internal errors to buffer
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <edhoc.h>
#include <unity.h>

#include "common/com_data_models.h"
#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"
#include "edhoc/common/add_error/environments/tst_edhoc_add_internal_error_env.h"

static struct tst_edhoc_add_internal_error_env env = {
    .error_buffer_view = {.capacity = TST_EDHOC_ADD_ERROR_CAPACITY}};

void setUp(void) { tst_edhoc_add_internal_error_reset_env(&env); }

static void assert_status_ok(
    const enum com_edhoc_add_error_status add_error_status) {
  tst_edhoc_assert_add_error_status_ok(add_error_status,
                                       env.error_buffer_view.length);
}

static void assert_encoded_error_matches(
    const char* expected_error_description) {
  tst_edhoc_assert_encoded_error_matches(env.error_buffer_view,
                                         expected_error_description,
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_add_internal_error(void) {
  const char* expected_error_description = "RANDOM DESCRIPTION";

  const enum com_edhoc_add_error_status add_error_status =
      com_edhoc_add_internal_error(expected_error_description,
                                   &env.error_buffer_view);

  assert_status_ok(add_error_status);
  assert_encoded_error_matches(expected_error_description);
}

void test_add_internal_error_creates_valid_error_on_null_message(void) {
  const enum com_edhoc_add_error_status add_error_status =
      com_edhoc_add_internal_error(NULL, &env.error_buffer_view);

  assert_status_ok(add_error_status);
  tst_edhoc_assert_encoded_error_is_not_empty(env.error_buffer_view);
}
