/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 27/05/2026
 * @brief Tests for the shared CLI argument parser.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <unity.h>

#include "common/com_parse_suites_arguments.h"

void test_ok_on_valid_data(void) {
  char* arguments[] = {"2", "0"};
  const size_t size = sizeof(arguments) / sizeof(arguments[0]);

  const struct com_parse_suites_arguments_result result =
      com_parse_suites_arguments(arguments, size);

  TEST_ASSERT_TRUE(result.success);
  TEST_ASSERT_EQUAL_size_t(2, result.suites.count);
  TEST_ASSERT_EQUAL_INT(2, result.suites.identifiers[0]);
  TEST_ASSERT_EQUAL_INT(0, result.suites.identifiers[1]);
}

void test_fails_on_invalid_data(void) {
  char* arguments[] = {"asda", "0"};
  const size_t size = sizeof(arguments) / sizeof(arguments[0]);

  const struct com_parse_suites_arguments_result result =
      com_parse_suites_arguments(arguments, size);

  TEST_ASSERT_FALSE(result.success);
}
