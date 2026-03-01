#include "psa_setup.h"

psa_status_t setup_psa_crypto(const psa_key_usage_t key_usage,
                              const uint8_t* key_material, size_t key_length,
                              psa_key_id_t* output_key_id) {
  psa_status_t status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    printf("PSA crypto init failed: %d\n", status);
    return status;
  }

  // Set up key attributes
  psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
  psa_set_key_usage_flags(&key_attributes, key_usage);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_CCM);
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_AES);
  const size_t key_bits = key_length * 8;
  psa_set_key_bits(&key_attributes, key_bits);

  // Import the key
  status =
      psa_import_key(&key_attributes, key_material, key_length, output_key_id);
  if (status != PSA_SUCCESS) {
    printf("PSA key import failed: %d\n", status);
    return status;
  }
  return PSA_SUCCESS;
}
