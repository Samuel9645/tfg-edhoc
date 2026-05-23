
#include "edhoc/common/com_edhoc_setup_context.h"

#include <edhoc_cipher_suite_2.h>
#include <psa/crypto.h>

#include "edhoc/common/com_logging.h"

static struct com_edhoc_setup_context_result ok(void) {
  return (struct com_edhoc_setup_context_result){
      .status = COM_EDHOC_SETUP_CTX_OK,
  };
}

static struct com_edhoc_setup_context_result failure(
    const enum com_edhoc_setup_context_status status) {
  return (struct com_edhoc_setup_context_result){.status = status};
}

struct com_edhoc_setup_context_result com_edhoc_setup_context(
    struct edhoc_context* context,
    const struct com_edhoc_parameters edhoc_parameters) {
  const struct com_edhoc_validate_parameters_result validation_result =
      com_edhoc_validate_parameters(&edhoc_parameters);
  if (validation_result.valid_parameters == false) {
    return failure(COM_EDHOC_SETUP_CTX_ERR_INVALID_EDHOC_PARAMETERS);
  }

  if (psa_crypto_init() != PSA_SUCCESS) {
    com_edhoc_log_error(
        "Context Setup error: PSA crypto initialization failed");
    return failure(COM_EDHOC_SETUP_CTX_ERR_PSA_INIT);
  }
  if (edhoc_context_init(context) != EDHOC_SUCCESS) {
    com_edhoc_log_error(
        "Context Setup error: EDHOC context initialization failed");
    return failure(COM_EDHOC_SETUP_CTX_ERR_CONTEXT_INIT);
  }
  if (edhoc_set_methods(context, edhoc_parameters.methods.data,
                        edhoc_parameters.methods.size) != EDHOC_SUCCESS) {
    com_edhoc_log_error("Context Setup error: Failed to set EDHOC methods");
    return failure(COM_EDHOC_SETUP_CTX_ERR_SET_METHODS);
  }
  const struct com_edhoc_cipher_suite_list supported_suites =
      edhoc_parameters.supported_cipher_suites;
  const size_t number_of_suites = supported_suites.number_of_suites;
  struct edhoc_cipher_suite cipher_suites[number_of_suites];
  const struct com_edhoc_cipher_suite_details* selected_cipher_suite_details =
      edhoc_parameters.selected_cipher_suite;
  const struct edhoc_cipher_suite* selected_suite =
      selected_cipher_suite_details->metadata;
  size_t write_index = 0;
  for (size_t i = 0; i < number_of_suites; i++) {
    const struct edhoc_cipher_suite* current_supported_suite =
        supported_suites.suites[i]->metadata;
    if (current_supported_suite == selected_suite) {
      continue;
    }
    cipher_suites[write_index++] = *current_supported_suite;
  }
  cipher_suites[number_of_suites - 1] = *selected_suite;
  if (edhoc_set_cipher_suites(context, cipher_suites, number_of_suites) !=
      EDHOC_SUCCESS) {
    com_edhoc_log_error("Context Setup error: Failed to set cipher suites");
    return failure(COM_EDHOC_SETUP_CTX_ERR_SET_CIPHER_SUITES);
  }
  const struct edhoc_connection_id connection_id = {
      .encode_type = EDHOC_CID_TYPE_ONE_BYTE_INTEGER,
      .int_value = edhoc_parameters.generate_connection_id(),
      .bstr_length = 0,
  };
  if (edhoc_set_connection_id(context, &connection_id) != EDHOC_SUCCESS) {
    com_edhoc_log_error("Context Setup error: Failed to set connection ID");
    return failure(COM_EDHOC_SETUP_CTX_ERR_SET_CONNECTION_ID);
  }

  const struct edhoc_keys* keys = selected_cipher_suite_details->get_keys();
  if (edhoc_bind_keys(context, keys) != EDHOC_SUCCESS) {
    com_edhoc_log_error("Context Setup error: Failed to bind keys");
    return failure(COM_EDHOC_SETUP_CTX_ERR_BIND_KEYS);
  }
  if (edhoc_set_user_context(context, (void*)keys) != EDHOC_SUCCESS) {
    com_edhoc_log_error("Context Setup error: Failed to set user context");
    return failure(COM_EDHOC_SETUP_CTX_ERR_SET_USER_CONTEXT);
  }
  if (edhoc_bind_crypto(context, selected_cipher_suite_details->get_crypto()) !=
      EDHOC_SUCCESS) {
    com_edhoc_log_error("Context Setup error: Failed to bind crypto");
    return failure(COM_EDHOC_SETUP_CTX_ERR_BIND_CRYPTO);
  }
  if (edhoc_bind_credentials(context, edhoc_parameters.credentials) !=
      EDHOC_SUCCESS) {
    com_edhoc_log_error("Context Setup error: Failed to bind credentials");
    return failure(COM_EDHOC_SETUP_CTX_ERR_BIND_CREDENTIALS);
  }
  return ok();
}