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

static struct com_edhoc_parameters create_test_params(
    const struct com_edhoc_cipher_suite_list suites,
    const struct com_edhoc_methods methods) {
  return (struct com_edhoc_parameters){
      .credentials = &DUMMY_TEST_CREDS,
      .supported_cipher_suites = suites,
      .selected_cipher_suite = suites.suites[0],
      .methods = methods};
}

struct com_edhoc_parameters tst_edhoc_srv_get_method_3_suite_2_params(void) {
  static const enum edhoc_method ONLY_METHOD_3[] = {EDHOC_METHOD_3};

  return create_test_params(
      COM_EDHOC_ONLY_SUITE_2,
      (struct com_edhoc_methods){.data = ONLY_METHOD_3, .size = 1});
}

struct com_edhoc_parameters tst_edhoc_srv_get_method_0_suite_0_params(void) {
  static const enum edhoc_method ONLY_METHOD_0[] = {EDHOC_METHOD_0};

  return create_test_params(COM_EDHOC_ONLY_SUITE_0, (struct com_edhoc_methods){
                                                        .data = ONLY_METHOD_0,
                                                        .size = 1,
                                                    });
}
