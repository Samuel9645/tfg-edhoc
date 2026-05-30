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

static const uint8_t CRED_EDHOC_CLI_PRIVATE_KEY[] = {
    0x97, 0x45, 0x6c, 0x36, 0x90, 0x89, 0x0e, 0xc0, 0xa5, 0x06, 0x01,
    0x69, 0xf8, 0xc0, 0xb1, 0x80, 0xf8, 0xe3, 0xd8, 0x12, 0x3f, 0xf7,
    0xd0, 0x92, 0x3f, 0x34, 0x77, 0xb6, 0x9a, 0xea, 0x21, 0x0f};

static const size_t CRED_EDHOC_CLI_PRIVATE_KEY_LENGTH =
    sizeof(CRED_EDHOC_CLI_PRIVATE_KEY);

static int client_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  const struct cred_get_edhoc_public_data_result public_data_result =
      cred_get_client_public_data();
  if (!public_data_result.credentials_found) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }
  return cred_edhoc_auth_fetch(
      user_context, credentials, public_data_result.public_credentials,
      CRED_EDHOC_CLI_PRIVATE_KEY, CRED_EDHOC_CLI_PRIVATE_KEY_LENGTH);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef (libedhoc signature
// forces it)
static int client_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  const struct cred_get_edhoc_public_data_result public_data_result =
      cred_get_server_public_data();
  if (!public_data_result.credentials_found) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }
  return cred_edhoc_auth_verify(user_context, credentials,
                                public_data_result.public_credentials,
                                public_key_reference, public_key_length);
}

struct edhoc_credentials get_client_credentials(void) {
  return (struct edhoc_credentials){
      .fetch = client_credential_fetch,
      .verify = client_credential_verify,
  };
}