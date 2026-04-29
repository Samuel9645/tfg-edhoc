/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 11/04/2026
 * @brief Testing for the happy path with the real libedhoc of functions to add
 * internal errors to buffer
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <edhoc.h>
#include <string.h>
#include <unity.h>

#include "common/com_data_models.h"
#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"
#include "edhoc/common/add_error/common/tst_edhoc_add_error_capacity.h"

static struct {
  uint8_t error_message[TST_EDHOC_ADD_ERROR_CAPACITY];
  struct com_writable_buffer error_buffer_view;
} env = {.error_buffer_view = {.capacity = TST_EDHOC_ADD_ERROR_CAPACITY}};

void setUp(void) {
  memset(env.error_message, 0, sizeof(env.error_message));
  env.error_buffer_view.bytes = env.error_message;
}

void test_add_internal_error(void) {
  const char* expected_description = "RANDOM DESCRIPTION";

  const struct com_edhoc_add_error_result result =
      com_edhoc_add_internal_error_result(expected_description,
                                          env.error_buffer_view);

  tst_edhoc_assert_add_error_status_ok(result);
  tst_edhoc_assert_encoded_error_matches(result.buffer, expected_description,
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_add_internal_error_creates_valid_error_on_null_message(void) {
  const struct com_edhoc_add_error_result result =
      com_edhoc_add_internal_error_result(NULL, env.error_buffer_view);

  tst_edhoc_assert_add_error_status_ok(result);
  tst_edhoc_assert_encoded_error_is_not_empty(result.buffer);
}
