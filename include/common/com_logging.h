/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 23/05/2026
 * @brief Declarations for the logging module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef COMMON_COM_LOGGING_H_
#define COMMON_COM_LOGGING_H_

/**
 * @brief Log an error message to the standard error output.
 * @param[in] format Null-terminated string containing a printf-style format
 * string, followed by any additional arguments required by the format string.
 * @param[in] ... Additional arguments required by the format string.
 * @note This function is a wrapper around fprintf(stderr, ...), it is used to
 * log error messages in a consistent way across the codebase. The format string
 * and arguments are passed directly to fprintf, so they should follow the same
 * syntax and rules as printf-style format strings.
 */
void com_log_error(const char* format, ...)
    __attribute__((format(printf, 1, 2)));

#endif  // COMMON_COM_LOGGING_H_