#ifndef PSA_SETUP_H_
#define PSA_SETUP_H_

#include <psa/crypto.h>
#include <psa/crypto_struct.h>
#include <psa/crypto_types.h>
#include <psa/crypto_values.h>
#include <stddef.h>

psa_status_t setup_psa_crypto(const psa_key_usage_t usage, const uint8_t* key,
                              size_t key_len, psa_key_id_t* key_id);

#endif  // PSA_SETUP_H_
