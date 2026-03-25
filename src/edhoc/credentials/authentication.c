#include "edhoc/credentials/authentication.h"

#include <edhoc_cipher_suite_2.h>
#include <edhoc_values.h>

#include "edhoc/common/setup.h"

int edhoc_credential_fetch(void* user_context, struct edhoc_auth_creds* credentials,
                     const uint8_t* own_public_key,
                     size_t own_public_key_length,
                     const uint8_t* own_private_key,
                     size_t own_private_key_length, int32_t own_key_id) {
  if (credentials == NULL) {
    return EDHOC_ERROR_INVALID_ARGUMENT;
  }

  edhoc_initialize_credential_key(credentials, own_public_key, own_public_key_length,
                            own_key_id);

  if (edhoc_cipher_suite_2_key_import(
          user_context, EDHOC_KT_SIGNATURE, own_private_key,
          own_private_key_length, credentials->priv_key_id) != EDHOC_SUCCESS) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }

  return EDHOC_SUCCESS;
}

int edhoc_credential_verify(void* user_context, struct edhoc_auth_creds* credentials,
                      int32_t expected_key_id, const uint8_t* peer_public_key,
                      size_t peer_public_key_length,
                      const uint8_t** public_key_reference,
                      size_t* public_key_length) {
  (void)user_context;
  if (credentials == NULL) {
    return EDHOC_ERROR_INVALID_ARGUMENT;
  }

  if (EDHOC_COSE_HEADER_KID != credentials->label ||
      EDHOC_ENCODE_TYPE_INTEGER != credentials->key_id.encode_type ||
      expected_key_id != credentials->key_id.key_id_int) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }

  *public_key_reference = peer_public_key;
  *public_key_length = peer_public_key_length;

  edhoc_initialize_credential_key(credentials, peer_public_key,
                            peer_public_key_length, expected_key_id);

  return EDHOC_SUCCESS;
}
