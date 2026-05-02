
#include "edhoc/common/com_edhoc_context_setup.h"

#include <edhoc_cipher_suite_2.h>
#include <psa/crypto.h>

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

static struct com_edhoc_setup_context_result invalid_error_buffer(void) {
  return (struct com_edhoc_setup_context_result){
      .status = COM_EDHOC_SETUP_CTX_ERR_INVALID_ERROR_BUFFER};
}

static struct com_edhoc_setup_context_result ok(void) {
  return (struct com_edhoc_setup_context_result){
      .status = COM_EDHOC_SETUP_CTX_OK,
  };
}

static struct com_edhoc_setup_context_result failure(
    const enum com_edhoc_setup_context_status status,
    const struct com_readonly_buffer error_buffer) {
  return (struct com_edhoc_setup_context_result){.status = status,
                                                 .error_buffer = error_buffer};
}

struct com_edhoc_setup_context_result com_edhoc_setup_context(
    struct edhoc_context* context,
    const struct srv_edhoc_parameters edhoc_parameters,
    const struct com_writable_buffer error_buffer) {
  if (!com_writable_buffer_is_writable(error_buffer)) {
    return invalid_error_buffer();
  }
  const struct com_edhoc_cipher_suite_list supported_suites =
      edhoc_parameters.supported_cipher_suites;
  if (!com_edhoc_cipher_suites_are_valid(supported_suites)) {
    return failure(COM_EDHOC_SETUP_CTX_ERR_INVALID_SUPPORTED_SUITES,
                   com_edhoc_add_internal_error_view(
                       "Context Setup error: Invalid supported cipher suites",
                       error_buffer));
  }
  if (edhoc_parameters.methods.data == NULL ||
      edhoc_parameters.methods.size == 0) {
    return failure(
        COM_EDHOC_SETUP_CTX_ERR_INVALID_METHODS,
        com_edhoc_add_internal_error_view(
            "Context Setup error: Invalid EDHOC methods", error_buffer));
  }
  if (edhoc_parameters.credentials == NULL) {
    return failure(COM_EDHOC_SETUP_CTX_ERR_NULL_CREDENTIALS,
                   com_edhoc_add_internal_error_view(
                       "Context Setup error: Null credentials", error_buffer));
  }

  if (psa_crypto_init() != PSA_SUCCESS) {
    return failure(COM_EDHOC_SETUP_CTX_ERR_PSA_INIT,
                   com_edhoc_add_internal_error_view(
                       "Context Setup error: PSA crypto initialization failed",
                       error_buffer));
  }
  if (edhoc_context_init(context) != EDHOC_SUCCESS) {
    return failure(
        COM_EDHOC_SETUP_CTX_ERR_CONTEXT_INIT,
        com_edhoc_add_internal_error_view(
            "Context Setup error: EDHOC context initialization failed",
            error_buffer));
  }
  if (edhoc_set_methods(context, edhoc_parameters.methods.data,
                        edhoc_parameters.methods.size) != EDHOC_SUCCESS) {
    return failure(
        COM_EDHOC_SETUP_CTX_ERR_SET_METHODS,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Context Setup error: Failed to set EDHOC methods",
            error_buffer));
  }
  const size_t number_of_suites = supported_suites.number_of_suites;
  struct edhoc_cipher_suite cipher_suites[number_of_suites];
  for (size_t i = 0; i < number_of_suites; i++) {
    cipher_suites[i] = *supported_suites.suites[i]->metadata;
  }
  if (edhoc_set_cipher_suites(context, cipher_suites, number_of_suites) !=
      EDHOC_SUCCESS) {
    return failure(
        COM_EDHOC_SETUP_CTX_ERR_SET_CIPHER_SUITES,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Context Setup error: Failed to set cipher suites",
            error_buffer));
  }
  const struct edhoc_connection_id connection_id = {
      .encode_type = EDHOC_CID_TYPE_ONE_BYTE_INTEGER,
      .int_value = 21,
      .bstr_length = 0,
  };
  if (edhoc_set_connection_id(context, &connection_id) != EDHOC_SUCCESS) {
    return failure(
        COM_EDHOC_SETUP_CTX_ERR_SET_CONNECTION_ID,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Context Setup error: Failed to set connection ID",
            error_buffer));
  }
  const struct com_edhoc_cipher_suite_details* first_suite_details =
      supported_suites.suites[0];
  if (edhoc_bind_keys(context, first_suite_details->get_keys()) !=
      EDHOC_SUCCESS) {
    return failure(
        COM_EDHOC_SETUP_CTX_ERR_BIND_KEYS,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Context Setup error: Failed to bind keys", error_buffer));
  }
  if (edhoc_bind_crypto(context, first_suite_details->get_crypto()) !=
      EDHOC_SUCCESS) {
    return failure(COM_EDHOC_SETUP_CTX_ERR_BIND_CRYPTO,
                   com_edhoc_add_protocol_error_with_description_view(
                       context, "Context Setup error: Failed to bind crypto",
                       error_buffer));
  }
  if (edhoc_bind_credentials(context, edhoc_parameters.credentials) !=
      EDHOC_SUCCESS) {
    return failure(
        COM_EDHOC_SETUP_CTX_ERR_BIND_CREDENTIALS,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Context Setup error: Failed to bind credentials",
            error_buffer));
  }
  return ok();
}