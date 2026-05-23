/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 29/04/2026
 * @brief Tests for the EDHOC setup context function
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "edhoc/common/com_edhoc_setup_context.h"
#include "edhoc/common/setup_context/mocks/tst_com_mock_edhoc_setup_context.h"
#include "edhoc/common/tst_edhoc_params.h"

enum { TST_SETUP_CONTEXT_BUFFER_SIZE = 256 };

static uint8_t error_buffer[TST_SETUP_CONTEXT_BUFFER_SIZE];

static struct {
  struct edhoc_context context;
} env = {0};

static void reset_env(void) {
  tst_com_reset_setup_context_mock();
  memset(error_buffer, 0, sizeof(error_buffer));
  memset(&env.context, 0, sizeof(env.context));
}

void setUp(void) { reset_env(); }

void test_ok_with_valid_parameters(void) {
  const struct com_edhoc_parameters params =
      tst_edhoc_get_method_0_suite_0_params();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_OK, result.status);
}

void test_fails_on_invalid_edhoc_parameters(void) {
  const struct com_edhoc_parameters params = {0};

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_INVALID_EDHOC_PARAMETERS,
                    result.status);
}

void test_fails_on_psa_crypto_init_failure(void) {
  const struct com_edhoc_parameters params =
      tst_edhoc_get_method_0_suite_0_params();
  tst_com_set_psa_crypto_init_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_PSA_INIT, result.status);
}

void test_fails_on_edhoc_context_init_failure(void) {
  const struct com_edhoc_parameters params =
      tst_edhoc_get_method_0_suite_0_params();
  tst_com_set_edhoc_context_init_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_CONTEXT_INIT, result.status);
}

void test_fails_on_set_methods_failure(void) {
  const struct com_edhoc_parameters params =
      tst_edhoc_get_method_0_suite_0_params();
  tst_com_set_edhoc_set_methods_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_SET_METHODS, result.status);
}

void test_fails_on_set_cipher_suites_failure(void) {
  const struct com_edhoc_parameters params =
      tst_edhoc_get_method_0_suite_0_params();
  tst_com_set_edhoc_set_cipher_suites_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_SET_CIPHER_SUITES, result.status);
}

void test_fails_on_set_connection_id_failure(void) {
  const struct com_edhoc_parameters params =
      tst_edhoc_get_method_0_suite_0_params();
  tst_com_set_edhoc_set_connection_id_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_SET_CONNECTION_ID, result.status);
}

void test_fails_on_bind_keys_failure(void) {
  const struct com_edhoc_parameters params =
      tst_edhoc_get_method_0_suite_0_params();
  tst_com_set_edhoc_bind_keys_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_BIND_KEYS, result.status);
}

void test_fails_on_set_user_context_failure(void) {
  const struct com_edhoc_parameters params =
      tst_edhoc_get_method_0_suite_0_params();
  tst_com_set_edhoc_set_user_context_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_SET_USER_CONTEXT, result.status);
}

void test_fails_on_bind_crypto_failure(void) {
  const struct com_edhoc_parameters params =
      tst_edhoc_get_method_0_suite_0_params();
  tst_com_set_edhoc_bind_crypto_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_BIND_CRYPTO, result.status);
}

void test_fails_on_bind_credentials_failure(void) {
  const struct com_edhoc_parameters params =
      tst_edhoc_get_method_0_suite_0_params();
  tst_com_set_edhoc_bind_credentials_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_BIND_CREDENTIALS, result.status);
}
