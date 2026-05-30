/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Definitions for the module to facilitate the creation of
 * edhoc_credentials members
 * @note The reason why it appears like the initialization is made twice
 * in the fetching and verifying is that the edhoc library uses the same
 * structure for both the own credentials and the peer credentials.
 * So in the fetch we initialize the structure with our own public key, and in
 * verify we initialize it with the peer public key. This is a design choice of
 * the library to simplify the handling of credentials, but it may seem a bit
 * redundant at first glance.
 */

#include "edhoc/credentials/cred_auth.h"

int cred_edhoc_auth_fetch(
    void* user_context, struct edhoc_auth_creds* credentials,
    const struct cred_public_credentials_data own_public_credentials,
    const uint8_t* own_private_key, const size_t own_private_key_length) {
  if (credentials == NULL) {
    return EDHOC_ERROR_INVALID_ARGUMENT;
  }

  credentials->label = EDHOC_COSE_HEADER_KID;
  credentials->key_id.cred = own_public_credentials.public_key.key;
  credentials->key_id.cred_len = own_public_credentials.public_key.length;
  credentials->key_id.cred_is_cbor = false;
  credentials->key_id.encode_type = EDHOC_ENCODE_TYPE_INTEGER;
  credentials->key_id.key_id_int = own_public_credentials.key_id;

  const struct edhoc_keys* keys = user_context;
  if (keys->import_key(user_context, EDHOC_KT_SIGNATURE, own_private_key,
                       own_private_key_length,
                       credentials->priv_key_id) != EDHOC_SUCCESS) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }

  return EDHOC_SUCCESS;
}

int cred_edhoc_auth_verify(
    const void* user_context, struct edhoc_auth_creds* credentials,
    const struct cred_public_credentials_data expected_peer_credentials,
    const uint8_t** public_key_reference, size_t* public_key_length) {
  (void)user_context;
  if (credentials == NULL) {
    return EDHOC_ERROR_INVALID_ARGUMENT;
  }

  if (EDHOC_COSE_HEADER_KID != credentials->label ||
      EDHOC_ENCODE_TYPE_INTEGER != credentials->key_id.encode_type ||
      expected_peer_credentials.key_id != credentials->key_id.key_id_int) {
    return EDHOC_ERROR_CREDENTIALS_FAILURE;
  }

  *public_key_reference = expected_peer_credentials.public_key.key;
  *public_key_length = expected_peer_credentials.public_key.length;

  credentials->key_id.cred = expected_peer_credentials.public_key.key;
  credentials->key_id.cred_len = expected_peer_credentials.public_key.length;
  credentials->key_id.cred_is_cbor = false;

  return EDHOC_SUCCESS;
}
