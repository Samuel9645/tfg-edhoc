
#include "edhoc/common/setup.h"

#include <edhoc_cipher_suite_2.h>
#include <psa/crypto.h>
#include <stdint.h>
#include <stdio.h>

int edhoc_common_setup_context(struct edhoc_context* context,
                        const struct edhoc_credentials* credentials) {
  const psa_status_t psa_status = psa_crypto_init();
  if (psa_status != PSA_SUCCESS) {
    fprintf(stderr, "cannot initialize PSA crypto: %d\n", psa_status);
    return psa_status;
  }
  int ret = edhoc_context_init(context);
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "cannot initialize context: %d\n", ret);
    return ret;
  }

  const enum edhoc_method methods[] = {EDHOC_METHOD_0};
  ret = edhoc_set_methods(context, methods, ARRAY_SIZE(methods));
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "cannot set methods: %d\n", ret);
    return ret;
  }

  const struct edhoc_cipher_suite cipher_suite_2 = {
      .value = 2,
      .aead_key_length = 16,
      .aead_tag_length = 8,
      .aead_iv_length = 13,
      .hash_length = 32,
      .mac_length = 32,
      .ecc_key_length = 32,
      .ecc_sign_length = 64,
  };
  const struct edhoc_cipher_suite cipher_suites[] = {cipher_suite_2};
  ret = edhoc_set_cipher_suites(context, cipher_suites,
                                ARRAY_SIZE(cipher_suites));
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "cannot set cipher suites: %d\n", ret);
    return ret;
  }

  const struct edhoc_connection_id connection_id = {
      .encode_type = EDHOC_CID_TYPE_ONE_BYTE_INTEGER,
      .int_value = 21,
      .bstr_length = 0,
  };
  ret = edhoc_set_connection_id(context, &connection_id);
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "cannot set connection ID: %d\n", ret);
    return ret;
  }

  ret = edhoc_bind_keys(context, edhoc_cipher_suite_2_get_keys());
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "cannot bind keys: %d\n", ret);
    return ret;
  }

  ret = edhoc_bind_crypto(context, edhoc_cipher_suite_2_get_crypto());
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "cannot bind crypto: %d\n", ret);
    return ret;
  }

  ret = edhoc_bind_credentials(context, credentials);
  if (ret != EDHOC_SUCCESS) {
    fprintf(stderr, "cannot bind credentials: %d\n", ret);
  }

  return ret;
}

void edhoc_common_initialize_credential_key(struct edhoc_auth_creds* credentials,
                               const uint8_t* public_key,
    const size_t public_key_length, const int32_t key_id_integer) {
  credentials->label = EDHOC_COSE_HEADER_KID;
  credentials->key_id.cred = public_key;
  credentials->key_id.cred_len = public_key_length;
  credentials->key_id.cred_is_cbor = false;
  credentials->key_id.encode_type = EDHOC_ENCODE_TYPE_INTEGER;
  credentials->key_id.key_id_int = key_id_integer;
}