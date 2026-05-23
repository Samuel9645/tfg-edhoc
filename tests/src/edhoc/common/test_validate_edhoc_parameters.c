/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/05/2026
 * @brief Test for the parameter validation function
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <string.h>
#include <unity.h>

#include "edhoc/common/com_edhoc_parameters.h"
#include "edhoc/common/tst_edhoc_params.h"

static uint8_t error_buffer[512];

void setUp(void) { memset(error_buffer, 0, sizeof(error_buffer)); }

void test_valid_parameters(void) {
  const struct com_edhoc_parameters valid_params =
      tst_edhoc_get_method_0_suite_0_params();

  const struct com_edhoc_validate_parameters_result result =
      com_edhoc_validate_parameters(&valid_params);

  TEST_ASSERT_TRUE(result.valid_parameters);
}

void test_fails_on_invalid_parameters(void) {
  const struct com_edhoc_parameters valid_params =
      tst_edhoc_get_method_0_suite_0_params();
  const struct com_edhoc_cipher_suite_list duplicated_cipher_suites = {
      .suites =
          (const struct com_edhoc_cipher_suite_details*[]){
              valid_params.supported_cipher_suites.suites[0],
              valid_params.supported_cipher_suites.suites[0],
          },
      .number_of_suites = 2,
  };

  const struct {
    struct com_edhoc_parameters params;
  } test_cases[] = {
      {
          .params =
              {
                  .methods = valid_params.methods,
                  .credentials = valid_params.credentials,
                  .selected_cipher_suite =
                      valid_params.supported_cipher_suites.suites[0],
                  .generate_connection_id = valid_params.generate_connection_id,
              },

      },

      {
          .params =
              {
                  .supported_cipher_suites =
                      valid_params.supported_cipher_suites,
                  .credentials = valid_params.credentials,
                  .selected_cipher_suite =
                      valid_params.supported_cipher_suites.suites[0],
                  .generate_connection_id = valid_params.generate_connection_id,
              },

      },

      {
          .params =
              {
                  .supported_cipher_suites =
                      valid_params.supported_cipher_suites,
                  .methods = valid_params.methods,
                  .selected_cipher_suite =
                      valid_params.supported_cipher_suites.suites[0],
                  .generate_connection_id = valid_params.generate_connection_id,
              },

      },
      {
          .params =
              {
                  .supported_cipher_suites =
                      valid_params.supported_cipher_suites,
                  .methods = valid_params.methods,
                  .credentials = valid_params.credentials,
                  .generate_connection_id = valid_params.generate_connection_id,
              },

      },
      {
          .params =
              {
                  .supported_cipher_suites =
                      valid_params.supported_cipher_suites,
                  .methods = valid_params.methods,
                  .credentials = valid_params.credentials,
                  .selected_cipher_suite = &COM_EDHOC_SUITE_2,
                  .generate_connection_id = valid_params.generate_connection_id,
              },

      },
      {.params =
           {
               .supported_cipher_suites = duplicated_cipher_suites,
               .methods = valid_params.methods,
               .credentials = valid_params.credentials,
               .selected_cipher_suite = valid_params.selected_cipher_suite,
               .generate_connection_id = valid_params.generate_connection_id,
           },
      },
      {
          .params =
              {
                  .methods = valid_params.methods,
                  .credentials = valid_params.credentials,
                  .supported_cipher_suites =
                      valid_params.supported_cipher_suites,
                  .selected_cipher_suite =
                      valid_params.supported_cipher_suites.suites[0],

              },

      },
  };

  const size_t test_length = sizeof(test_cases) / sizeof(test_cases[0]);

  for (size_t i = 0; i < test_length; i++) {
    setUp();

    const struct com_edhoc_validate_parameters_result result =
        com_edhoc_validate_parameters(&test_cases[i].params);

    TEST_ASSERT_FALSE(result.valid_parameters);
  }
}