#include "shared_credentials.h"

#include "edhoc_cipher_suite_2.h"
#include "edhoc_macros.h"
#include "edhoc_setup.h"
#include "edhoc_values.h"

int shared_credential_fetch(void* user_context,
                            struct edhoc_auth_creds* credentials,
                            const uint8_t* public_key, size_t public_key_length,
                            const uint8_t* private_key,
                            size_t private_key_length, int32_t key_id) {
  if (credentials == NULL)
    return EDHOC_ERROR_INVALID_ARGUMENT;

  initialize_credential_key(credentials, public_key, public_key_length, key_id);

  int ret = edhoc_cipher_suite_2_key_import(user_context, EDHOC_KT_SIGNATURE,
                                            private_key, private_key_length,
                                            credentials->priv_key_id);

  if (ret != EDHOC_SUCCESS) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }

  return EDHOC_SUCCESS;
}

int shared_credential_verify(void* user_context,
                             struct edhoc_auth_creds* credentials,
                             int32_t expected_key_id,
                             const uint8_t* peer_public_key,
                             size_t peer_public_key_length,
                             const uint8_t** public_key_reference,
                             size_t* public_key_length) {
  (void)user_context;
  if (NULL == credentials)
    return EDHOC_ERROR_INVALID_ARGUMENT;

  if (EDHOC_COSE_HEADER_KID != credentials->label)
    return EDHOC_ERROR_CREDENTIALS_FAILURE;

  if (EDHOC_ENCODE_TYPE_INTEGER != credentials->key_id.encode_type)
    return EDHOC_ERROR_CREDENTIALS_FAILURE;

  if (expected_key_id != credentials->key_id.key_id_int)
    return EDHOC_ERROR_CREDENTIALS_FAILURE;

  *public_key_reference = peer_public_key;
  *public_key_length = peer_public_key_length;

  return EDHOC_SUCCESS;
}
