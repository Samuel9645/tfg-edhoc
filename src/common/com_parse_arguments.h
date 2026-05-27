/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 27/05/2026
 * @brief Declarations for the module to parse main program arguments.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef COMMON_COM_PARSE_ARGUMENTS_H_
#define COMMON_COM_PARSE_ARGUMENTS_H_
#include <stdbool.h>
#include <stddef.h>

enum { COM_MAX_SUITES_IDENTIFIERS = 3 };

struct com_cipher_suite_identifiers {
  int identifiers[COM_MAX_SUITES_IDENTIFIERS];
  size_t count;
};

struct com_arguments {
  struct com_cipher_suite_identifiers supported_suites;
  struct com_cipher_suite_identifiers preferred_suites;
};

struct com_parse_arguments_result {
  bool success;
  const struct com_arguments arguments;
};

struct com_parse_arguments_result parse_arguments(char* arguments[],
                                                  size_t size);

#endif  // COMMON_COM_PARSE_ARGUMENTS_H_
