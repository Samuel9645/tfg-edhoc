/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 27/04/2026
 * @brief Declarations for the parameters needed in EDHOC
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_COMMON_COM_EDHOC_PARAMETERS_H_
#define EDHOC_COMMON_COM_EDHOC_PARAMETERS_H_

#include "edhoc/common/com_edhoc_cipher_suites.h"

struct srv_edhoc_parameters {
  const struct edhoc_credentials* credentials;
  const struct com_edhoc_cipher_suite_list* supported_cipher_suites;
};
#endif  // EDHOC_COMMON_COM_EDHOC_PARAMETERS_H_