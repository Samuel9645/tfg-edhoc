
#include "edhoc/common/com_edhoc_context_setup.h"

#include <edhoc_cipher_suite_2.h>
#include <psa/crypto.h>

enum com_edhoc_setup_context_status com_edhoc_setup_context(
    struct edhoc_context* context,
    const struct srv_edhoc_parameters edhoc_parameters) {
  const struct com_edhoc_cipher_suite_list* supported_suites =
      edhoc_parameters.supported_cipher_suites;
  if (!com_edhoc_cipher_suites_are_valid(supported_suites)) {
    return COM_EDHOC_SETUP_CTX_ERR_INVALID_SUPPORTED_SUITES;
  }
  if (edhoc_parameters.methods.data == NULL ||
      edhoc_parameters.methods.size == 0) {
    return COM_EDHOC_SETUP_CTX_ERR_INVALID_METHODS;
  }
  if (edhoc_parameters.credentials == NULL) {
    return COM_EDHOC_SETUP_CTX_ERR_NULL_CREDENTIALS;
  }

  if (psa_crypto_init() != PSA_SUCCESS) {
    return COM_EDHOC_SETUP_CTX_ERR_PSA_INIT;
  }
  if (edhoc_context_init(context) != EDHOC_SUCCESS) {
    return COM_EDHOC_SETUP_CTX_ERR_CONTEXT_INIT;
  }
  if (edhoc_set_methods(context, edhoc_parameters.methods.data,
                        edhoc_parameters.methods.size) != EDHOC_SUCCESS) {
    return COM_EDHOC_SETUP_CTX_ERR_SET_METHODS;
  }
  const size_t number_of_suites = supported_suites->number_of_suites;
  struct edhoc_cipher_suite cipher_suites[number_of_suites];
  for (size_t i = 0; i < number_of_suites; i++) {
    cipher_suites[i] = *supported_suites->suites[i].metadata;
  }
  if (edhoc_set_cipher_suites(context, cipher_suites, number_of_suites) !=
      EDHOC_SUCCESS) {
    return COM_EDHOC_SETUP_CTX_ERR_SET_CIPHER_SUITES;
  }
  const struct edhoc_connection_id connection_id = {
      .encode_type = EDHOC_CID_TYPE_ONE_BYTE_INTEGER,
      .int_value = 21,
      .bstr_length = 0,
  };
  if (edhoc_set_connection_id(context, &connection_id) != EDHOC_SUCCESS) {
    return COM_EDHOC_SETUP_CTX_ERR_SET_CONNECTION_ID;
  }
  const struct com_edhoc_cipher_suite_details* first_suite_details =
      &supported_suites->suites[0];
  if (edhoc_bind_keys(context, first_suite_details->get_keys()) !=
      EDHOC_SUCCESS) {
    return COM_EDHOC_SETUP_CTX_ERR_BIND_KEYS;
  }
  if (edhoc_bind_crypto(context, first_suite_details->get_crypto()) !=
      EDHOC_SUCCESS) {
    return COM_EDHOC_SETUP_CTX_ERR_BIND_CRYPTO;
  }
  if (edhoc_bind_credentials(context, edhoc_parameters.credentials) !=
      EDHOC_SUCCESS) {
    return COM_EDHOC_SETUP_CTX_ERR_BIND_CREDENTIALS;
  }
  return COM_EDHOC_SETUP_CTX_OK;
}