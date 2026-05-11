
#include "edhoc/common/com_edhoc_setup_context.h"

#include <edhoc_cipher_suite_2.h>
#include <psa/crypto.h>

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

/**
 * @brief The range of valid CIDs for the session, using single byte ids,
 * according to RFC 9528.
 * @see [RFC
 * 9528 3.3.2](https://datatracker.ietf.org/doc/html/rfc9528#name-representation-of-byte-stri)
 */
enum {
  // WHY DON'T WE USE THIS?
  // libedhoc (v3.16) doesn't decode integers when specifying
  // EDHOC_CID_TYPE_ONE_BYTE_INTEGER, but it simply cast them, without encoding
  // it, meaning that we cannot use negative values since these are different
  // from the CBOR representation.
  COM_EDHOC_MIN_INT_CID = -24,

  COM_EDHOC_LIBEDHOC_MIN_INT_CID = 0,
  COM_EDHOC_MAX_INT_CID = 23
};

static int8_t cid_counter = COM_EDHOC_LIBEDHOC_MIN_INT_CID;

/**
 * @brief Gets the next valid one byte connection identifier of the session
 * @see [RFC
 * 9528 3.3.2](https://datatracker.ietf.org/doc/html/rfc9528#name-representation-of-byte-stri)
 * @see [RFC
 * 9528 3.3.3](https://datatracker.ietf.org/doc/html/rfc9528#name-use-of-connection-identifie)
 * @warning This is a simulation so wrapping the value is fine since the session
 * will never have more than 48 messages, but in a real implementation this
 * should be implemented in a way that guarantees uniqueness of the CID for the
 * session, as explained in RFC 9528 3.3.3.
 * @return Connection Identifier of the new session.
 */
static int8_t get_next_single_byte_integer_cid(void) {
  const int8_t assigned = cid_counter;
  cid_counter++;
  if (cid_counter > COM_EDHOC_MAX_INT_CID) {
    cid_counter = COM_EDHOC_LIBEDHOC_MIN_INT_CID;
  }
  return assigned;
}

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
    const struct com_edhoc_parameters edhoc_parameters,
    const struct com_writable_buffer error_buffer) {
  if (!com_writable_buffer_is_writable(error_buffer)) {
    return invalid_error_buffer();
  }
  const struct com_edhoc_validate_parameters_result validation_result =
      com_edhoc_validate_parameters(&edhoc_parameters, error_buffer);
  if (validation_result.valid_parameters == false) {
    return failure(COM_EDHOC_SETUP_CTX_ERR_INVALID_EDHOC_PARAMETERS,
                   validation_result.error_message);
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
    return failure(
        COM_EDHOC_SETUP_CTX_ERR_SET_CIPHER_SUITES,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Context Setup error: Failed to set cipher suites",
            error_buffer));
  }
  const struct edhoc_connection_id connection_id = {
      .encode_type = EDHOC_CID_TYPE_ONE_BYTE_INTEGER,
      .int_value = get_next_single_byte_integer_cid(),
      .bstr_length = 0,
  };
  if (edhoc_set_connection_id(context, &connection_id) != EDHOC_SUCCESS) {
    return failure(
        COM_EDHOC_SETUP_CTX_ERR_SET_CONNECTION_ID,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Context Setup error: Failed to set connection ID",
            error_buffer));
  }
  if (edhoc_bind_keys(context, selected_cipher_suite_details->get_keys()) !=
      EDHOC_SUCCESS) {
    return failure(
        COM_EDHOC_SETUP_CTX_ERR_BIND_KEYS,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Context Setup error: Failed to bind keys", error_buffer));
  }
  if (edhoc_bind_crypto(context, selected_cipher_suite_details->get_crypto()) !=
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