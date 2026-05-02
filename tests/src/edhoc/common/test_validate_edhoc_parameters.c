/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/05/2026
 * @brief Test for the parameter validation function
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <string.h>
#include <unity.h>

#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"
#include "edhoc/common/com_edhoc_parameters.h"
#include "edhoc/common/tst_edhoc_params.h"

static uint8_t error_buffer[100];
static const struct com_writable_buffer error_buffer_view = {
    .bytes = error_buffer, .capacity = sizeof(error_buffer)};

void setUp(void) { memset(error_buffer, 0, sizeof(error_buffer)); }

void test_valid_parameters(void) {
  const struct srv_edhoc_parameters valid_params =
      tst_edhoc_srv_get_method_0_suite_0_params();

  const struct srv_edhoc_validate_parameters_result result =
      com_edhoc_validate_parameters(&valid_params, error_buffer_view);

  TEST_ASSERT_TRUE(result.valid_parameters);
}

void test_invalid_supported_cipher_suites(void) {
  const struct srv_edhoc_parameters valid_params =
      tst_edhoc_srv_get_method_0_suite_0_params();
  const struct com_edhoc_cipher_suite_list invalid_suites = {
      .number_of_suites = 1, .suites = NULL};
  const struct srv_edhoc_parameters params = {
      .credentials = valid_params.credentials,
      .supported_cipher_suites = invalid_suites,
      .methods = valid_params.methods};

  const struct srv_edhoc_validate_parameters_result result =
      com_edhoc_validate_parameters(&params, error_buffer_view);

  TEST_ASSERT_FALSE(result.valid_parameters);
  tst_edhoc_assert_encoded_error_matches(
      result.error_message,
      "EDHOC parameters validation error: Invalid supported cipher suites",
      EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_invalid_methods(void) {
  const struct srv_edhoc_parameters valid_params =
      tst_edhoc_srv_get_method_0_suite_0_params();
  const struct srv_edhoc_methods invalid_methods = {.data = NULL, .size = 0};
  const struct srv_edhoc_parameters params = {
      .credentials = valid_params.credentials,
      .supported_cipher_suites = valid_params.supported_cipher_suites,
      .methods = invalid_methods};

  const struct srv_edhoc_validate_parameters_result result =
      com_edhoc_validate_parameters(&params, error_buffer_view);

  TEST_ASSERT_FALSE(result.valid_parameters);
  tst_edhoc_assert_encoded_error_matches(
      result.error_message,
      "EDHOC parameters validation error: Invalid EDHOC methods",
      EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_invalid_credentials(void) {
  const struct srv_edhoc_parameters valid_params =
      tst_edhoc_srv_get_method_0_suite_0_params();
  const struct srv_edhoc_parameters params = {
      .credentials = NULL,
      .supported_cipher_suites = valid_params.supported_cipher_suites,
      .methods = valid_params.methods};

  const struct srv_edhoc_validate_parameters_result result =
      com_edhoc_validate_parameters(&params, error_buffer_view);

  TEST_ASSERT_FALSE(result.valid_parameters);
  tst_edhoc_assert_encoded_error_matches(
      result.error_message,
      "EDHOC parameters validation error: Null credentials",
      EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}