/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Declarations for the module to get public data credentials
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CREDENTIALS_CRED_PUB_DATA_H_
#define EDHOC_CREDENTIALS_CRED_PUB_DATA_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "edhoc/common/com_edhoc_cipher_suites.h"

struct cred_public_credentials_data {
  const struct {
    const uint8_t* const key;
    const size_t length;
  } public_key;

  const int32_t key_id;
};

struct cred_get_edhoc_public_data_result {
  bool credentials_found;
  struct cred_public_credentials_data public_credentials;
};

struct cred_get_edhoc_public_data_result cred_get_client_public_data(
    enum com_edhoc_cipher_suite_identifier cipher_suite_identifier);

struct cred_get_edhoc_public_data_result cred_get_server_public_data(
    enum com_edhoc_cipher_suite_identifier cipher_suite_identifier);

#endif  // EDHOC_CREDENTIALS_CRED_PUB_DATA_H_
