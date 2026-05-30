/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Definitions for the module to get the credentials of the client
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/credentials/cred_cli.h"

#include "edhoc/credentials/cred_auth.h"
#include "edhoc/credentials/cred_pub_data.h"

static const uint8_t CLI_SUITE_2_PRIVATE_KEY[] = {
    0x97, 0x45, 0x6c, 0x36, 0x90, 0x89, 0x0e, 0xc0, 0xa5, 0x06, 0x01,
    0x69, 0xf8, 0xc0, 0xb1, 0x80, 0xf8, 0xe3, 0xd8, 0x12, 0x3f, 0xf7,
    0xd0, 0x92, 0x3f, 0x34, 0x77, 0xb6, 0x9a, 0xea, 0x21, 0x0f};

static const uint8_t CLI_SUITE_0_PRIVATE_KEY[] = {
    0x39, 0x83, 0x0c, 0x4a, 0x5a, 0x35, 0x23, 0xf6, 0xf9, 0xa6, 0x22,
    0xc7, 0x85, 0x0a, 0x39, 0x87, 0x5e, 0x8f, 0x1c, 0x1e, 0x57, 0xe9,
    0x15, 0x77, 0xb9, 0x84, 0x36, 0xc8, 0x94, 0xea, 0x47, 0xc5, 0xee,
    0xf7, 0x52, 0x2e, 0x29, 0xdb, 0x09, 0x3c, 0x74, 0xc1, 0xb3, 0x65,
    0x57, 0xf5, 0x20, 0x98, 0x8a, 0x18, 0xc8, 0x61, 0xce, 0xe4, 0xac,
    0x66, 0xf7, 0x84, 0xdf, 0xfa, 0x05, 0xeb, 0xa0, 0x98};

static int client_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  const struct com_edhoc_cipher_suite_details* cipher_suite_details =
      user_context;
  const struct cred_get_edhoc_public_data_result public_data_result =
      cred_get_client_public_data(cipher_suite_details->metadata->value);
  if (!public_data_result.credentials_found) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }
  const uint8_t* client_private_key = NULL;
  size_t client_private_key_length = 0;
  switch (cipher_suite_details->metadata->value) {
  case COM_EDHOC_CIPHER_SUITE_0_IDENTIFIER:
    client_private_key = CLI_SUITE_0_PRIVATE_KEY;
    client_private_key_length = sizeof(CLI_SUITE_0_PRIVATE_KEY);
    break;
  case COM_EDHOC_CIPHER_SUITE_2_IDENTIFIER:
    client_private_key = CLI_SUITE_2_PRIVATE_KEY;
    client_private_key_length = sizeof(CLI_SUITE_2_PRIVATE_KEY);
    break;
  default:
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }
  return cred_edhoc_auth_fetch(credentials, cipher_suite_details->get_keys(),
                               public_data_result.public_credentials,
                               client_private_key, client_private_key_length);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef (libedhoc signature
// forces it)
static int client_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  const struct com_edhoc_cipher_suite_details* cipher_suite_details =
      user_context;
  const struct cred_get_edhoc_public_data_result public_data_result =
      cred_get_server_public_data(cipher_suite_details->metadata->value);
  if (!public_data_result.credentials_found) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }
  return cred_edhoc_auth_verify(credentials,
                                public_data_result.public_credentials,
                                public_key_reference, public_key_length);
}

struct edhoc_credentials get_client_credentials(void) {
  return (struct edhoc_credentials){
      .fetch = client_credential_fetch,
      .verify = client_credential_verify,
  };
}