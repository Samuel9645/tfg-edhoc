/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 27/05/2026
 * @brief Declarations for the module to parse main program arguments.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef COMMON_com_parse_suite_argumentsS_H_
#define COMMON_com_parse_suite_argumentsS_H_
#include <stdbool.h>
#include <stddef.h>

enum { COM_MAX_SUITES_IDENTIFIERS = 3 };

struct com_cipher_suite_identifiers {
  int identifiers[COM_MAX_SUITES_IDENTIFIERS];
  size_t count;
};

struct com_parse_suites_arguments_result {
  bool success;

  const struct {
    struct com_cipher_suite_identifiers supported_suites;
    struct com_cipher_suite_identifiers preferred_suites;
  } arguments;
};

/**
 * @brief Parses arguments with the following format:
 * -p|--preferred [ <id1> <id2> ...] -s|--supported [ <id1> <id2> ...]
 * @param[in] arguments Array of strings with the arguments to parse
 * @param[in] size Size of the arguments array
 * @return Struct containing the result of the parsing, including a success flag
 * and the parsed arguments if successful, or an empty arguments struct with
 * success false on failure.
 */
struct com_parse_suites_arguments_result com_parse_suites_arguments(
    char* arguments[], size_t size);

#endif  // COMMON_com_parse_suite_argumentsS_H_
