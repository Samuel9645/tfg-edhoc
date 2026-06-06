/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 27/05/2026
 * @brief Definitions for the module to parse main program arguments.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "common/com_parse_suites_arguments.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "common/com_logging.h"

struct com_parse_identifier_result {
  bool success;
  int selected_suite_identifier;
};

static struct com_parse_identifier_result parse_identifier(const char* text) {
  if (text == NULL) {
    return (struct com_parse_identifier_result){.success = false};
  }

  errno = 0;
  char* end = NULL;
  const long parsed_value = strtol(text, &end, 10);
  if (errno != 0 || end == text || *end != '\0' || parsed_value < INT_MIN ||
      parsed_value > INT_MAX) {
    return (struct com_parse_identifier_result){.success = false};
  }
  return (struct com_parse_identifier_result){
      .success = true, .selected_suite_identifier = (int)parsed_value};
}

static bool append_suite_identifier(
    struct com_cipher_suite_identifiers* suite_list, const int identifier) {
  if (suite_list->count >= COM_MAX_SUITES_IDENTIFIERS) {
    com_log_error("Too many suites. Max is %d\n", COM_MAX_SUITES_IDENTIFIERS);
    return false;
  }

  suite_list->identifiers[suite_list->count] = identifier;
  suite_list->count++;
  return true;
}

struct com_parse_suites_arguments_result com_parse_suites_arguments(
    char* arguments[], const size_t size) {
  if (arguments == NULL || size < 1) {
    com_log_error("Invalid arguments: arguments array is NULL or too small\n");
    return (struct com_parse_suites_arguments_result){.success = false};
  }
  struct com_cipher_suite_identifiers suites = {0};
  for (size_t i = 0; i < size; i++) {
    const char* current_argument = arguments[i];
    const struct com_parse_identifier_result parse_result =
        parse_identifier(current_argument);
    if (!parse_result.success) {
      com_log_error("Invalid cipher suite identifier: %s\n", current_argument);
      return (struct com_parse_suites_arguments_result){.success = false};
    }
    if (!append_suite_identifier(&suites,
                                 parse_result.selected_suite_identifier)) {
      return (struct com_parse_suites_arguments_result){.success = false};
    }
  }
  return (struct com_parse_suites_arguments_result){
      .success = true,
      .suites = suites,
  };
}
