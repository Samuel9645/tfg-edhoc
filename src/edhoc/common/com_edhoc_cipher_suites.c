/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 27/04/2026
 * @brief Definitions for pre-arranged suite packages and cipher suite structs
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/common/com_edhoc_cipher_suites.h"

#include <edhoc.h>
#include <edhoc_cipher_suite_2.h>

static bool suites_details_are_valid(
    const struct com_edhoc_cipher_suite_details* details) {
  if (details == NULL) {
    return false;
  }
  return details->metadata != NULL || details->get_keys != NULL ||
         details->get_crypto != NULL;
}

bool com_edhoc_cipher_suites_are_valid(
    const struct com_edhoc_cipher_suite_list* details) {
  if (details == NULL) {
    return false;
  }
  return details->number_of_suites != 0 &&
         suites_details_are_valid(details->suites);
}

const struct com_edhoc_cipher_suite_details COM_EDHOC_SUITE_0;
static const struct edhoc_cipher_suite cipher_suite_2 = {
    .value = 2,
    .aead_key_length = 16,
    .aead_tag_length = 8,
    .aead_iv_length = 13,
    .hash_length = 32,
    .mac_length = 32,
    .ecc_key_length = 32,
    .ecc_sign_length = 64,
};

const struct com_edhoc_cipher_suite_details COM_EDHOC_SUITE_2 = {
    .metadata = &cipher_suite_2,
    .get_keys = edhoc_cipher_suite_2_get_keys,
    .get_crypto = edhoc_cipher_suite_2_get_crypto,
};

const struct com_edhoc_cipher_suite_list COM_EDHOC_ONLY_SUITE_2 = {
    .number_of_suites = 1,
    .suites = &COM_EDHOC_SUITE_2,
};
