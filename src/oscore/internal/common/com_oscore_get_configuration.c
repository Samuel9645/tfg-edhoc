/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 26/05/2026
 * @brief Definitions of the functions to get the OSCORE configuration using the
 * EDHOC context
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "oscore/internal/common/com_oscore_get_configuration.h"

#include <coap3/coap.h>
#include <stdio.h>

struct srv_oscore_algorithms {
  const bool success;
  const char* aead_algorithm;
  const char* hkdf_algorithm;
};

/**
 * @brief Extract the algorithms to use for OSCORE session derivation from the
 * EDHOC context, based on the selected cipher suite.
 * @param context EDHOC context to extract the selected cipher suite and
 * determine the algorithms to use for OSCORE session derivation
 * @return Struct containing the names of the algorithms to use for the OSCORE
 * session and a status of the operation
 * @see [RFC
 * 9528 10.2](https://datatracker.ietf.org/doc/html/rfc9528#name-edhoc-cipher-suites-registr)
 */
static struct srv_oscore_algorithms get_algorithms(
    const struct edhoc_context* context) {
  switch (context->private_csuite[context->private_chosen_csuite_idx].value) {
  case 0:
  case 2:
    return (struct srv_oscore_algorithms){
        .aead_algorithm = "AES-CCM-16-64-128",
        .hkdf_algorithm = "direct+HKDF-SHA-256",
        .success = true};
  default:
    return (struct srv_oscore_algorithms){0};
  }
}

enum {
  HEX_CHARS_PER_BYTE = 2,
};

static void bytes_to_hex_string(const uint8_t* bytes, const size_t length,
                                char* output) {
  for (size_t i = 0; i < length; i++) {
    sprintf(output + i * HEX_CHARS_PER_BYTE, "%02x", bytes[i]);
  }
}

bool com_oscore_build_config(
    struct edhoc_context* edhoc_context,
    const struct com_writable_char_buffer configuration_buffer) {
  if (edhoc_context == NULL) {
    return false;
  }

  enum {
    EDHOC_AES_CCM_KEY_SIZE_BYTES = 16,
    EDHOC_OSCORE_SALT_SIZE_BYTES = 8,
    EDHOC_MAX_CONNECTION_ID_SIZE_BYTES = 8
  };

  enum { STRING_TERMINATOR_SPACE = 1 };

  enum {
    OSCORE_SECRET_HEX_BUF_SIZE =
        EDHOC_AES_CCM_KEY_SIZE_BYTES * HEX_CHARS_PER_BYTE +
        STRING_TERMINATOR_SPACE,
    OSCORE_SALT_HEX_BUF_SIZE =
        EDHOC_OSCORE_SALT_SIZE_BYTES * HEX_CHARS_PER_BYTE +
        STRING_TERMINATOR_SPACE,
    OSCORE_ID_HEX_BUF_SIZE =
        EDHOC_MAX_CONNECTION_ID_SIZE_BYTES * HEX_CHARS_PER_BYTE +
        STRING_TERMINATOR_SPACE,
  };

  uint8_t master_secret[EDHOC_AES_CCM_KEY_SIZE_BYTES];
  uint8_t master_salt[EDHOC_OSCORE_SALT_SIZE_BYTES];
  uint8_t sender_id[EDHOC_MAX_CONNECTION_ID_SIZE_BYTES];
  uint8_t recipient_id[EDHOC_MAX_CONNECTION_ID_SIZE_BYTES];
  size_t sender_id_len = 0;
  size_t recipient_id_len = 0;

  if (edhoc_export_oscore_session(
          edhoc_context, master_secret, sizeof(master_secret), master_salt,
          sizeof(master_salt), sender_id, sizeof(sender_id), &sender_id_len,
          recipient_id, sizeof(recipient_id),
          &recipient_id_len) != EDHOC_SUCCESS) {
    coap_log_err(
        "OSCORE Build Configuration Error: Failed to export secrets from EDHOC "
        "context\n");
    return false;
  }

  char secret_hex[OSCORE_SECRET_HEX_BUF_SIZE] = {0};
  char salt_hex[OSCORE_SALT_HEX_BUF_SIZE] = {0};
  char sender_hex[OSCORE_ID_HEX_BUF_SIZE] = {0};
  char recipient_hex[OSCORE_ID_HEX_BUF_SIZE] = {0};

  bytes_to_hex_string(master_secret, sizeof(master_secret), secret_hex);
  bytes_to_hex_string(master_salt, sizeof(master_salt), salt_hex);
  bytes_to_hex_string(sender_id, sender_id_len, sender_hex);
  bytes_to_hex_string(recipient_id, recipient_id_len, recipient_hex);
  const struct srv_oscore_algorithms algorithms = get_algorithms(edhoc_context);
  if (!algorithms.success) {
    coap_log_err(
        "OSCORE Build Configuration Error: Unsupported cipher suite for OSCORE "
        "session derivation\n");
    return false;
  }

  if (configuration_buffer.capacity < COM_OSCORE_CONFIG_CSV_MAX_SIZE) {
    coap_log_err(
        "OSCORE Build Configuration Error: Configuration buffer is too small "
        "to hold the configuration\n");
    return false;
  }
  snprintf(configuration_buffer.data, configuration_buffer.capacity,
           "master_secret,hex,%s\n"
           "master_salt,hex,%s\n"
           "sender_id,hex,%s\n"
           "recipient_id,hex,%s\n"
           "aead_alg,text,%s\n"
           "hkdf_alg,text,%s\n",
           secret_hex, salt_hex, sender_hex, recipient_hex,
           algorithms.aead_algorithm, algorithms.hkdf_algorithm);
  return true;
}