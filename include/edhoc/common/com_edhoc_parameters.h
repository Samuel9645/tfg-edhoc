/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 27/04/2026
 * @brief Declarations for the parameters needed in EDHOC
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_COMMON_COM_EDHOC_PARAMETERS_H_
#define EDHOC_COMMON_COM_EDHOC_PARAMETERS_H_

#include <edhoc.h>

#include "common/com_data_models.h"
#include "edhoc/common/com_edhoc_cipher_suites.h"

struct com_edhoc_methods {
  const enum edhoc_method* data;
  const size_t size;
};

struct com_edhoc_parameters {
  const struct edhoc_credentials* credentials;
  const struct com_edhoc_cipher_suite_list supported_cipher_suites;
  const struct com_edhoc_cipher_suite_details* selected_cipher_suite;
  const struct com_edhoc_methods methods;
};

struct com_edhoc_validate_parameters_result {
  bool valid_parameters;
  const struct com_readonly_buffer error_message;
};

/**
 * @brief Validates the given parameters for setting up an EDHOC context,
 * writing an error message to the provided buffer if they are invalid.
 * @param parameters Pointer to the parameters struct to validate
 * @param error_buffer View of the buffer to write the error into
 * @return Struct containing valid_parameters as true and empty error message on
 * success, false valid_parameters and a view of the error message on failure
 */
struct com_edhoc_validate_parameters_result com_edhoc_validate_parameters(
    const struct com_edhoc_parameters* parameters,
    struct com_writable_buffer error_buffer);
#endif  // EDHOC_COMMON_COM_EDHOC_PARAMETERS_H_