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

static bool argument_is_supported_tag(const char* current_argument) {
  return current_argument != NULL &&
         (strcmp(current_argument, "-s") == 0 ||
          strcmp(current_argument, "--supported") == 0);
}

static bool argument_is_preferred_tag(const char* current_argument) {
  return current_argument != NULL &&
         (strcmp(current_argument, "-p") == 0 ||
          strcmp(current_argument, "--preferred") == 0);
}

static bool append_suite_identifier(
    struct com_cipher_suite_identifiers* suite_list, const int identifier,
    const char* list_name) {
  if (suite_list->count >= COM_MAX_SUITES_IDENTIFIERS) {
    com_log_error("Too many %s suites. Max is %d\n", list_name,
                  COM_MAX_SUITES_IDENTIFIERS);
    return false;
  }

  suite_list->identifiers[suite_list->count] = identifier;
  suite_list->count++;
  return true;
}

void log_usage(void) {
  com_log_error(
      "Expected format: -p|--preferred [ <id1> <id2> ...] -s|--supported [ "
      "<id1> <id2> ...]\n");
}

void log_error_and_usage(const char* error_message) {
  com_log_error("%s", error_message);
  log_usage();
}

struct com_parse_suites_arguments_result com_parse_suites_arguments(
    char* arguments[], const size_t size) {
  if (arguments == NULL || size < 3) {
    log_error_and_usage(
        "Invalid arguments: arguments array is NULL or too small\n");
    return (struct com_parse_suites_arguments_result){.success = false};
  }
  if (!argument_is_preferred_tag(arguments[1])) {
    log_error_and_usage("First argument must be -p or --preferred\n");
    return (struct com_parse_suites_arguments_result){.success = false};
  }
  struct com_cipher_suite_identifiers preferred_suites_identifiers = {0};
  struct com_cipher_suite_identifiers supported_suites_identifiers = {0};

  enum { MODE_PREFERRED, MODE_SUPPORTED } current_mode = MODE_PREFERRED;

  for (size_t i = 2; i < size; i++) {
    const char* current_argument = arguments[i];
    if (argument_is_supported_tag(current_argument)) {
      current_mode = MODE_SUPPORTED;
      continue;
    }
    if (argument_is_preferred_tag(current_argument)) {
      log_error_and_usage("Preferred tag cannot appear after supported tag\n");
      return (struct com_parse_suites_arguments_result){.success = false};
    }
    const struct com_parse_identifier_result parse_result =
        parse_identifier(current_argument);
    if (!parse_result.success) {
      com_log_error("Invalid cipher suite identifier: %s\n", current_argument);
      log_usage();
      return (struct com_parse_suites_arguments_result){.success = false};
    }

    bool append_success = false;
    if (current_mode == MODE_PREFERRED) {
      append_success = append_suite_identifier(
          &preferred_suites_identifiers, parse_result.selected_suite_identifier,
          "preferred");
    } else {
      append_success = append_suite_identifier(
          &supported_suites_identifiers, parse_result.selected_suite_identifier,
          "supported");
    }
    if (!append_success) {
      return (struct com_parse_suites_arguments_result){.success = false};
    }
  }

  if (preferred_suites_identifiers.count == 0 ||
      supported_suites_identifiers.count == 0) {
    log_error_and_usage(
        "At least one preferred and one supported suite identifier must be "
        "provided\n");
    return (struct com_parse_suites_arguments_result){.success = false};
  }

  return (struct com_parse_suites_arguments_result){
      .success = true,
      .arguments = {
          .preferred_suites = preferred_suites_identifiers,
          .supported_suites = supported_suites_identifiers,
      }};
}
