/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 27/04/2026
 * @brief Declarations for the default values of the edhoc_params struct in
 * tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "edhoc/common/tst_edhoc_params.h"

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

struct com_edhoc_parameters tst_edhoc_create_test_params(
    const struct com_edhoc_cipher_suite_list suites,
    const struct com_edhoc_methods methods,
    const struct com_edhoc_cipher_suite_details* selected_suite) {
  return (struct com_edhoc_parameters){.credentials = &DUMMY_TEST_CREDS,
                                       .supported_cipher_suites = suites,
                                       .selected_cipher_suite = selected_suite,
                                       .methods = methods};
}

static const enum edhoc_method ONLY_METHOD_0_PTR[] = {EDHOC_METHOD_0};

static const struct com_edhoc_methods ONLY_METHOD_0 = {
    .data = ONLY_METHOD_0_PTR,
    .size = sizeof(ONLY_METHOD_0_PTR) / sizeof(ONLY_METHOD_0_PTR[0]),
};

struct com_edhoc_parameters tst_edhoc_get_method_0_suite_0_params(void) {
  return tst_edhoc_create_test_params(COM_EDHOC_ONLY_SUITE_0, ONLY_METHOD_0,
                                      &COM_EDHOC_SUITE_0);
}
