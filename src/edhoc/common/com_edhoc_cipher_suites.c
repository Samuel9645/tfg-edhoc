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
#include <stdlib.h>

#include "../../../include/common/com_logging.h"

static bool suites_details_are_valid(
    const struct com_edhoc_cipher_suite_details* details) {
  if (details == NULL) {
    return false;
  }
  return details->metadata != NULL || details->get_keys != NULL ||
         details->get_crypto != NULL;
}

bool com_edhoc_cipher_suites_are_valid(
    const struct com_edhoc_cipher_suite_list list) {
  if (list.suites == NULL || list.number_of_suites == 0) {
    return false;
  }

  for (size_t i = 0; i < list.number_of_suites; i++) {
    if (!suites_details_are_valid(list.suites[i])) {
      return false;
    }
  }
  return true;
}

const struct com_edhoc_cipher_suite_details*
com_edhoc_get_cipher_suite_from_identifier(const int identifier) {
  switch (identifier) {
  case 0:
    return &COM_EDHOC_SUITE_0;
  case 2:
    return &COM_EDHOC_SUITE_2;
  default:
    return NULL;
  }
}

struct com_edhoc_create_cipher_suites_result
com_edhoc_create_cipher_suites_from(const int* suites_identifiers,
                                    const size_t number_of_suites) {
  // ReSharper disable once CppDFAMemoryLeak, freed using
  // com_edhoc_delete_created_cipher_suite_list
  const struct com_edhoc_cipher_suite_details** suites_buffer = calloc(
      1, number_of_suites * sizeof(struct com_edhoc_cipher_suite_details*));
  if (suites_buffer == NULL) {
    com_log_error("Failed to allocate memory for cipher suites buffer\n");
    return (struct com_edhoc_create_cipher_suites_result){
        .success = false,
    };
  }
  for (size_t i = 0; i < number_of_suites; i++) {
    const struct com_edhoc_cipher_suite_details* suite_details =
        com_edhoc_get_cipher_suite_from_identifier(suites_identifiers[i]);
    if (suite_details == NULL) {
      free(suites_buffer);
      com_log_error(
          "Failed to create cipher suites: invalid suite identifier %d\n",
          suites_identifiers[i]);
      return (struct com_edhoc_create_cipher_suites_result){
          .success = false,
      };
    }
    suites_buffer[i] = suite_details;
  }
  // ReSharper disable once CppDFAMemoryLeak, freed using
  // com_edhoc_delete_created_cipher_suite_list
  return (struct com_edhoc_create_cipher_suites_result){
      .success = true,
      .cipher_suites = {
          .number_of_suites = number_of_suites,
          .suites = suites_buffer,
      }};
}

void com_edhoc_delete_created_cipher_suite_list(
    struct com_edhoc_cipher_suite_list* list) {
  if (list == NULL || list->suites == NULL) {
    return;
  }
  free(list->suites);
  list->suites = NULL;
  list->number_of_suites = 0;
}

static const struct edhoc_cipher_suite CIPHER_SUITE_0 = {
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
    .metadata = &CIPHER_SUITE_0,
    .get_crypto = edhoc_cipher_suite_0_get_crypto,
    .get_keys = edhoc_cipher_suite_0_get_keys,
};

static const struct com_edhoc_cipher_suite_details* ONLY_SUITE_0_DETAILS[] = {
    &COM_EDHOC_SUITE_0};
const struct com_edhoc_cipher_suite_list COM_EDHOC_ONLY_SUITE_0 = {
    .number_of_suites =
        sizeof(ONLY_SUITE_0_DETAILS) / sizeof(ONLY_SUITE_0_DETAILS[0]),
    .suites = ONLY_SUITE_0_DETAILS,
};

static const struct edhoc_cipher_suite CIPHER_SUITE_2 = {
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
    .metadata = &CIPHER_SUITE_2,
    .get_keys = edhoc_cipher_suite_2_get_keys,
    .get_crypto = edhoc_cipher_suite_2_get_crypto,
};

static const struct com_edhoc_cipher_suite_details* ONLY_SUITE_2_DETAILS[] = {
    &COM_EDHOC_SUITE_2};
const struct com_edhoc_cipher_suite_list COM_EDHOC_ONLY_SUITE_2 = {
    .number_of_suites =
        sizeof(ONLY_SUITE_2_DETAILS) / sizeof(ONLY_SUITE_2_DETAILS[0]),
    .suites = ONLY_SUITE_2_DETAILS,
};

static const struct com_edhoc_cipher_suite_details* SUITE_0_2_DETAILS[] = {
    &COM_EDHOC_SUITE_0, &COM_EDHOC_SUITE_2};

const struct com_edhoc_cipher_suite_list COM_EDHOC_SUITES_2_0 = {
    .number_of_suites =
        sizeof(SUITE_0_2_DETAILS) / sizeof(SUITE_0_2_DETAILS[0]),
    .suites = SUITE_0_2_DETAILS,
};
