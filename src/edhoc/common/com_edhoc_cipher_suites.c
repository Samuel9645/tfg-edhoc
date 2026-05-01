/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 27/04/2026
 * @brief Definitions for pre-arranged suite packages and cipher suite structs
 * @see [Github Repository](https:
 */

#include "edhoc/common/com_edhoc_cipher_suites.h"

#include <edhoc.h>
#include <edhoc_cipher_suite_0.h>
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
    const struct com_edhoc_cipher_suite_list* list) {
  if (list == NULL || list->suites == NULL || list->number_of_suites == 0) {
    return false;
  }

  for (size_t i = 0; i < list->number_of_suites; i++) {
    if (!suites_details_are_valid(list->suites[i])) {
      return false;
    }
  }
  return true;
}

static const struct edhoc_cipher_suite cipher_suite_0 = {
    .value = 0,
    .aead_key_length = 16,
    .aead_tag_length = 8,
    .aead_iv_length = 13,
    .hash_length = 32,
    .mac_length = 8,
    .ecc_key_length = 32,
    .ecc_sign_length = 64,
};
const struct com_edhoc_cipher_suite_details COM_EDHOC_SUITE_0 = {
    .metadata = &cipher_suite_0,
    .get_crypto = edhoc_cipher_suite_0_get_crypto,
    .get_keys = edhoc_cipher_suite_0_get_keys,
};

static const struct com_edhoc_cipher_suite_details* SUITE_0_PTRS[] = {
    &COM_EDHOC_SUITE_0};
const struct com_edhoc_cipher_suite_list COM_EDHOC_ONLY_SUITE_0 = {
    .number_of_suites = sizeof(SUITE_0_PTRS) / sizeof(SUITE_0_PTRS[0]),
    .suites = SUITE_0_PTRS,
};

static const struct edhoc_cipher_suite cipher_suite_2 = {
    .value = 2,
    .aead_key_length = 16,
    .aead_tag_length = 8,
    .aead_iv_length = 13,
    .hash_length = 32,
    .mac_length = 8,
    .ecc_key_length = 32,
    .ecc_sign_length = 64,
};

const struct com_edhoc_cipher_suite_details COM_EDHOC_SUITE_2 = {
    .metadata = &cipher_suite_2,
    .get_keys = edhoc_cipher_suite_2_get_keys,
    .get_crypto = edhoc_cipher_suite_2_get_crypto,
};

static const struct com_edhoc_cipher_suite_details* SUITE_2_PTRS[] = {
    &COM_EDHOC_SUITE_2};
const struct com_edhoc_cipher_suite_list COM_EDHOC_ONLY_SUITE_2 = {
    .number_of_suites = sizeof(SUITE_2_PTRS) / sizeof(SUITE_2_PTRS[0]),
    .suites = SUITE_2_PTRS,  // Apunta al array de direcciones
};
