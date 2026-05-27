/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 27/05/2026
 * @brief Tests for the shared CLI argument parser.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <unity.h>

#include "common/com_parse_arguments.h"

void test_ok_on_valid_data(void) {
  char* arguments[] = {"client", "-p", "2", "-s", "2", "0"};
  const size_t size = sizeof(arguments) / sizeof(arguments[0]);

  const struct com_parse_arguments_result result =
      parse_arguments(arguments, size);

  TEST_ASSERT_TRUE(result.success);
  TEST_ASSERT_EQUAL_size_t(2, result.arguments.supported_suites.count);
  TEST_ASSERT_EQUAL_size_t(1, result.arguments.preferred_suites.count);
  TEST_ASSERT_EQUAL_INT(2, result.arguments.supported_suites.identifiers[0]);
  TEST_ASSERT_EQUAL_INT(0, result.arguments.supported_suites.identifiers[1]);
  TEST_ASSERT_EQUAL_INT(2, result.arguments.preferred_suites.identifiers[0]);
}

void test_fails_on_invalid_data(void) {
  char* missing_preferred_tag[] = {"client", "2", "-s", "2", "0"};
  char* missing_supported_tag[] = {"client", "-p", "2", "2", "0"};
  char* missing_supported[] = {"client", "-p", "2", "-s"};
  char* missing_preferred[] = {"client", "-p", "-s", "2", "0"};
  char* non_numeric[] = {"client", "-p", "2", "-s", "2", "x"};

  const struct {
    const char* description;
    int size;
    char** arguments;
  } test_cases[] = {
      {"Fails with missing supported suites",
       sizeof(missing_supported) / sizeof(missing_supported[0]),
       missing_supported},
      {"Fails with missing preferred suites",
       sizeof(missing_preferred) / sizeof(missing_preferred[0]),
       missing_preferred},
      {"Fails with missing supported tag",
       sizeof(missing_supported_tag) / sizeof(missing_supported_tag[0]),
       missing_supported_tag},
      {"Fails with missing preferred tag",
       sizeof(missing_preferred_tag) / sizeof(missing_preferred_tag[0]),
       missing_preferred_tag},
      {"Fails with non-numeric selected suite",
       sizeof(non_numeric) / sizeof(non_numeric[0]), non_numeric}};

  const size_t num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

  for (size_t i = 0; i < num_cases; i++) {
    const struct com_parse_arguments_result result =
        parse_arguments(test_cases[i].arguments, test_cases[i].size);

    TEST_ASSERT_FALSE_MESSAGE(result.success, test_cases[i].description);
  }
}
