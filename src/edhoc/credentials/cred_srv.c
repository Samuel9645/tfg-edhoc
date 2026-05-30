/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Definitions for the module to get the credentials of the server
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/credentials/cred_srv.h"

#include "edhoc/credentials/cred_auth.h"
#include "edhoc/credentials/cred_pub_data.h"

static const uint8_t CRED_EDHOC_SRV_PRIVATE_KEY[] = {
    0xbc, 0xfc, 0x08, 0x24, 0x79, 0xa1, 0x90, 0x98, 0xf4, 0xd4, 0x9a,
    0xf5, 0x0b, 0xbe, 0x12, 0x92, 0xbd, 0xda, 0x1a, 0x8a, 0x92, 0xb1,
    0xcd, 0x75, 0xff, 0x42, 0x0d, 0x72, 0x0f, 0xc1, 0x94, 0xff};

static const size_t CRED_EDHOC_SRV_PRIVATE_KEY_LENGTH =
    sizeof(CRED_EDHOC_SRV_PRIVATE_KEY);

static int server_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  const struct cred_get_edhoc_public_data_result public_data_result =
      cred_get_server_public_data();
  if (!public_data_result.credentials_found) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }
  return cred_edhoc_auth_fetch(
      user_context, credentials, public_data_result.public_credentials,
      CRED_EDHOC_SRV_PRIVATE_KEY, CRED_EDHOC_SRV_PRIVATE_KEY_LENGTH);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
static int server_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  const struct cred_get_edhoc_public_data_result public_data_result =
      cred_get_client_public_data();
  if (!public_data_result.credentials_found) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }
  return cred_edhoc_auth_verify(user_context, credentials,
                                public_data_result.public_credentials,
                                public_key_reference, public_key_length);
}

struct edhoc_credentials get_server_credentials(void) {
  return (struct edhoc_credentials){
      .fetch = server_credential_fetch,
      .verify = server_credential_verify,
  };
}