/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 21/05/2026
 * @brief Definitions for the function to bind the OSCORE session to the
 * current CoAP context using EDHOC
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "../../../include/oscore/server/srv_oscore_bind_session.h"

#include <stdio.h>
#include <string.h>

static void bytes_to_hex_string(const uint8_t* bytes, const size_t length,
                                char* output, const size_t hex_multiplier) {
  for (size_t i = 0; i < length; i++) {
    sprintf(output + i * hex_multiplier, "%02x", bytes[i]);
  }
}

enum status_coap srv_oscore_bind_session(coap_context_t* coap_context,
                                         struct edhoc_context* edhoc_context) {
  enum {
    EDHOC_AES_CCM_KEY_SIZE_BYTES = 16,
    EDHOC_OSCORE_SALT_SIZE_BYTES = 8,
    EDHOC_MAX_CONNECTION_ID_SIZE_BYTES = 8
  };

  enum { HEX_CHARS_PER_BYTE = 2, STRING_TERMINATOR_SPACE = 1 };

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
    OSCORE_CONFIG_CSV_MAX_SIZE = 512
  };

  if (coap_context == NULL || edhoc_context == NULL) {
    return STATUS_COAP_ERR;
  }

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
        "OSCORE Binding: Failed to export secrets from EDHOC context (Err: "
        ")\n");
    return STATUS_COAP_ERR;
  }

  char secret_hex[OSCORE_SECRET_HEX_BUF_SIZE] = {0};
  char salt_hex[OSCORE_SALT_HEX_BUF_SIZE] = {0};
  char sender_hex[OSCORE_ID_HEX_BUF_SIZE] = {0};
  char recipient_hex[OSCORE_ID_HEX_BUF_SIZE] = {0};

  bytes_to_hex_string(master_secret, sizeof(master_secret), secret_hex,
                      HEX_CHARS_PER_BYTE);
  bytes_to_hex_string(master_salt, sizeof(master_salt), salt_hex,
                      HEX_CHARS_PER_BYTE);
  bytes_to_hex_string(sender_id, sender_id_len, sender_hex, HEX_CHARS_PER_BYTE);
  bytes_to_hex_string(recipient_id, recipient_id_len, recipient_hex,
                      HEX_CHARS_PER_BYTE);

  char configuration_text[OSCORE_CONFIG_CSV_MAX_SIZE] = {0};
  snprintf(configuration_text, sizeof(configuration_text),
           "master_secret,hex,%s\n"
           "master_salt,hex,%s\n"
           "sender_id,hex,%s\n"
           "recipient_id,hex,%s\n",
           secret_hex, salt_hex, sender_hex, recipient_hex);
  const coap_str_const_t* configuration_data =
      coap_make_str_const(configuration_text);

  coap_oscore_conf_t* oscore_configuration =
      coap_new_oscore_conf(*configuration_data, NULL, NULL, 0);
  if (oscore_configuration == NULL) {
    coap_log_err(
        "OSCORE Binding: libcoap failed to parse generated configuration "
        "text\n");
    return STATUS_COAP_ERR;
  }
  if (coap_context_oscore_server(coap_context, oscore_configuration) != 1) {
    coap_log_err(
        "OSCORE Binding: Failed to bind OSCORE configuration to the server "
        "context\n");
    coap_delete_oscore_conf(oscore_configuration);
    return STATUS_COAP_ERR;
  }
  coap_log_info("OSCORE Context successfully binded.\n");
  return STATUS_COAP_OK;
}
