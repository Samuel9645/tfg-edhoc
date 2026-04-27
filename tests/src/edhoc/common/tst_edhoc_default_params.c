/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 27/04/2026
 * @brief Declarations for the default values of the edhoc_params struct in
 * tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "edhoc/common/tst_edhoc_default_params.h"

#include <edhoc.h>

#include "edhoc/common/com_edhoc_cipher_suites.h"

static int dummy_fetch_credentials(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  (void)user_context;
  (void)credentials;
  return EDHOC_SUCCESS;
}

static int dummy_verify_credentials(
    void* user_context, struct edhoc_auth_creds* credentials,
    const uint8_t** public_key_reference,
    size_t* public_key_length) {  // NOLINT(*-non-const-parameter)
  (void)user_context;
  (void)credentials;
  (void)public_key_reference;
  (void)public_key_length;
  return EDHOC_SUCCESS;
}

static const struct edhoc_credentials DUMMY_TEST_CREDS = {
    .fetch = dummy_fetch_credentials, .verify = dummy_verify_credentials};

struct srv_edhoc_parameters tst_edhoc_srv_get_default_params(void) {
  static const struct srv_edhoc_parameters params = {
      .credentials = &DUMMY_TEST_CREDS,
      .supported_cipher_suites = &COM_EDHOC_ONLY_SUITE_2};
  return params;
}
