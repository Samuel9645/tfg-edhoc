#include "psa_setup.h"

psa_status_t setup_psa_crypto(const psa_key_usage_t usage, const uint8_t* key,
                              size_t key_len, psa_key_id_t* key_id) {
  psa_status_t status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    printf("PSA crypto init failed: %d\n", status);
    return status;
  }

  // Set up key attributes
  psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_usage_flags(&attr, usage);
  psa_set_key_algorithm(&attr, PSA_ALG_CCM);
  psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
  const size_t key_bits = key_len * 8;
  psa_set_key_bits(&attr, key_bits);

  // Import the key
  status = psa_import_key(&attr, key, key_len, key_id);
  if (status != PSA_SUCCESS) {
    printf("PSA key import failed: %d\n", status);
    return status;
  }
  return PSA_SUCCESS;
}
