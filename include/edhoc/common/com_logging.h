/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 23/05/2026
 * @brief Declarations for the logging module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_COMMON_COM_LOGGING_H_
#define EDHOC_COMMON_COM_LOGGING_H_

/**
 * @brief Log an error message to the standard error output.
 * @param[in] message Null-terminated string containing the error message to
 * log.
 */
void com_edhoc_log_error(const char* message);

#endif  // EDHOC_COMMON_COM_LOGGING_H_