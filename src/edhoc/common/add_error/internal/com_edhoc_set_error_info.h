/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 11/04/2026
 * @brief Declarations for the com_set_error_info module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_COMMON_ADD_ERROR_INTERNAL_COM_SET_ERROR_INFO_H_
#define EDHOC_COMMON_ADD_ERROR_INTERNAL_COM_SET_ERROR_INFO_H_

#include <edhoc.h>

/**
 * @brief Adds the error message into the error info struct
 * @param[out] error_info Pointer to the error info struct to set the error
 * message in
 * @param[in] error_message Null-terminated string containing the error message
 * to set
 * @note Struct will be zeroed before initialization
 */
void com_edhoc_set_error_info(struct edhoc_error_info* error_info,
                              const char* error_message);

#endif  // EDHOC_COMMON_ADD_ERROR_INTERNAL_COM_SET_ERROR_INFO_H_