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

static const uint8_t SRV_SUITE_2_PRIVATE_KEY[] = {
    0xbc, 0xfc, 0x08, 0x24, 0x79, 0xa1, 0x90, 0x98, 0xf4, 0xd4, 0x9a,
    0xf5, 0x0b, 0xbe, 0x12, 0x92, 0xbd, 0xda, 0x1a, 0x8a, 0x92, 0xb1,
    0xcd, 0x75, 0xff, 0x42, 0x0d, 0x72, 0x0f, 0xc1, 0x94, 0xff};

static const uint8_t SRV_SUITE_0_PRIVATE_KEY[] = {
    0x10, 0x14, 0x9a, 0x00, 0x81, 0xe5, 0x52, 0x23, 0xe8, 0x06, 0x03,
    0x67, 0x10, 0xbe, 0x9a, 0xd2, 0xf6, 0x2b, 0xac, 0xc2, 0x4a, 0x7f,
    0xcd, 0xb2, 0xfc, 0x6d, 0xde, 0x8d, 0xda, 0xf1, 0xc4, 0x50, 0x4c,
    0x8e, 0x1c, 0xc6, 0x51, 0xe9, 0x41, 0x2d, 0x73, 0x74, 0x8c, 0x02,
    0x0e, 0xc0, 0x39, 0x0f, 0xd5, 0xbd, 0x86, 0x50, 0x8d, 0x34, 0xe0,
    0x73, 0xb9, 0xb9, 0xc1, 0x36, 0x5a, 0xb2, 0xb5, 0xf4};

static int server_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  const struct com_edhoc_cipher_suite_details* cipher_suite_details =
      user_context;
  const struct cred_get_edhoc_public_data_result public_data_result =
      cred_get_server_public_data(cipher_suite_details->metadata->value);
  if (!public_data_result.credentials_found) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }
  const uint8_t* server_private_key = NULL;
  size_t server_private_key_length = 0;
  switch (cipher_suite_details->metadata->value) {
  case COM_EDHOC_CIPHER_SUITE_0_IDENTIFIER:
    server_private_key = SRV_SUITE_0_PRIVATE_KEY;
    server_private_key_length = sizeof(SRV_SUITE_0_PRIVATE_KEY);
    break;
  case COM_EDHOC_CIPHER_SUITE_2_IDENTIFIER:
    server_private_key = SRV_SUITE_2_PRIVATE_KEY;
    server_private_key_length = sizeof(SRV_SUITE_2_PRIVATE_KEY);
    break;
  default:
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }
  return cred_edhoc_auth_fetch(credentials, cipher_suite_details->get_keys(),
                               public_data_result.public_credentials,
                               server_private_key, server_private_key_length);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
static int server_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  const struct com_edhoc_cipher_suite_details* cipher_suite_details =
      user_context;
  const struct cred_get_edhoc_public_data_result public_data_result =
      cred_get_client_public_data(cipher_suite_details->metadata->value);
  if (!public_data_result.credentials_found) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }
  return cred_edhoc_auth_verify(credentials,
                                public_data_result.public_credentials,
                                public_key_reference, public_key_length);
}

struct edhoc_credentials get_server_credentials(void) {
  return (struct edhoc_credentials){
      .fetch = server_credential_fetch,
      .verify = server_credential_verify,
  };
}