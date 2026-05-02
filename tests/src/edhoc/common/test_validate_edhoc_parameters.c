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

static uint8_t error_buffer[512];
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

void test_fails_on_invalid_parameters(void) {
  const struct srv_edhoc_parameters valid_params =
      tst_edhoc_srv_get_method_0_suite_0_params();

  const struct {
    struct srv_edhoc_parameters params;
    const char* expected_error;
  } test_cases[] = {
      {
          .params = {.methods = valid_params.methods,
                     .credentials = valid_params.credentials,
                     .preferred_cipher_suites =
                         valid_params.preferred_cipher_suites,
                     .selected_cipher_suite =
                         valid_params.preferred_cipher_suites.suites[0]},
          .expected_error =
              "EDHOC parameters validation error: Invalid supported "
              "cipher suites",
      },

      {
          .params = {.supported_cipher_suites =
                         valid_params.supported_cipher_suites,
                     .credentials = valid_params.credentials,
                     .preferred_cipher_suites =
                         valid_params.preferred_cipher_suites,
                     .selected_cipher_suite =
                         valid_params.preferred_cipher_suites.suites[0]},
          .expected_error =
              "EDHOC parameters validation error: Invalid EDHOC methods",
      },

      {
          .params = {.supported_cipher_suites =
                         valid_params.supported_cipher_suites,
                     .methods = valid_params.methods,
                     .preferred_cipher_suites =
                         valid_params.preferred_cipher_suites,
                     .selected_cipher_suite =
                         valid_params.preferred_cipher_suites.suites[0]},
          .expected_error =
              "EDHOC parameters validation error: Null credentials",
      },
      {
          .params = {.supported_cipher_suites =
                         valid_params.supported_cipher_suites,
                     .methods = valid_params.methods,
                     .credentials = valid_params.credentials,
                     .selected_cipher_suite =
                         valid_params.preferred_cipher_suites.suites[0]

          },
          .expected_error = "EDHOC parameters validation error: Invalid "
                            "preferred cipher suites",
      },
      {
          .params = {.supported_cipher_suites =
                         valid_params.supported_cipher_suites,
                     .methods = valid_params.methods,
                     .preferred_cipher_suites = COM_EDHOC_ONLY_SUITE_2,
                     .credentials = valid_params.credentials,
                     .selected_cipher_suite =
                         valid_params.preferred_cipher_suites.suites[0]},
          .expected_error =
              "EDHOC parameters validation error: Preferred cipher suites must "
              "be a subset of supported cipher suites",
      },
      {
          .params =
              {
                  .supported_cipher_suites =
                      valid_params.supported_cipher_suites,
                  .preferred_cipher_suites =
                      valid_params.preferred_cipher_suites,
                  .methods = valid_params.methods,
                  .credentials = valid_params.credentials,
              },
          .expected_error = "EDHOC parameters validation error: Invalid "
                            "selected cipher suite",
      },
      {
          .params = {.supported_cipher_suites =
                         valid_params.supported_cipher_suites,
                     .methods = valid_params.methods,
                     .credentials = valid_params.credentials,
                     .preferred_cipher_suites =
                         valid_params.preferred_cipher_suites,
                     .selected_cipher_suite = &COM_EDHOC_SUITE_2},
          .expected_error = "EDHOC parameters validation error: Selected "
                            "cipher suite must be one of the supported ones",
      },
  };

  const size_t test_length = sizeof(test_cases) / sizeof(test_cases[0]);

  for (size_t i = 0; i < test_length; i++) {
    setUp();

    const struct srv_edhoc_validate_parameters_result result =
        com_edhoc_validate_parameters(&test_cases[i].params, error_buffer_view);

    TEST_ASSERT_FALSE(result.valid_parameters);
    tst_edhoc_assert_encoded_error_matches(result.error_message,
                                           test_cases[i].expected_error,
                                           EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
  }
}