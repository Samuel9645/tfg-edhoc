#include "shared_crypto.h"

#include "common_data.h"
#include "psa_setup.h"

psa_status_t shared_encrypt_plaintext(
    const uint8_t* plaintext_buffer, size_t plaintext_length,
    const uint8_t* key_material, size_t key_length, uint8_t* ciphertext_buffer,
    size_t ciphertext_buffer_size, size_t* ciphertext_output_length) {
  psa_key_id_t key_id;
  psa_status_t status = setup_psa_crypto(PSA_KEY_USAGE_ENCRYPT, key_material,
                                         key_length, &key_id);
  if (status != PSA_SUCCESS) {
    return status;
  }

  uint8_t nonce[NONCE_LENGTH_BYTES] = {0};
  // Encrypt with AES-CCM
  status =
      psa_aead_encrypt(key_id, PSA_ALG_CCM, nonce, sizeof(nonce), NULL,
                       0,  // No AAD
                       plaintext_buffer, plaintext_length, ciphertext_buffer,
                       ciphertext_buffer_size, ciphertext_output_length);

  // Clean up
  psa_destroy_key(key_id);
  return status;
}

psa_status_t shared_decrypt_ciphertext(
    const uint8_t* ciphertext_buffer, size_t ciphertext_length,
    const uint8_t* key_material, size_t key_length, uint8_t* plaintext_buffer,
    size_t plaintext_buffer_size, size_t* plaintext_output_length) {
  psa_key_id_t key_id;
  psa_status_t status = setup_psa_crypto(PSA_KEY_USAGE_DECRYPT, key_material,
                                         key_length, &key_id);
  if (status != PSA_SUCCESS) {
    return status;
  }

  uint8_t nonce[NONCE_LENGTH_BYTES] = {0};
  // Decrypt with AES-CCM
  status = psa_aead_decrypt(
      key_id, PSA_ALG_CCM, nonce, sizeof(nonce), NULL, 0,  // No AAD
      ciphertext_buffer, ciphertext_length, plaintext_buffer,
      plaintext_buffer_size, plaintext_output_length);
  // Clean up
  psa_destroy_key(key_id);
  return status;
}
