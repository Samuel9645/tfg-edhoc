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

#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"
#include "edhoc/common/com_edhoc_context_setup.h"
#include "edhoc/common/setup_context/mocks/tst_com_mock_edhoc_setup_context.h"
#include "edhoc/common/tst_edhoc_default_params.h"

enum { TST_SETUP_CONTEXT_BUFFER_SIZE = 256 };

static uint8_t error_buffer[TST_SETUP_CONTEXT_BUFFER_SIZE];

static struct {
  struct edhoc_context context;
  const struct com_writable_buffer error_buffer_view;
} env = {.error_buffer_view = {.bytes = error_buffer,
                               .capacity = sizeof(error_buffer)}};

static void reset_env(void) {
  tst_com_reset_setup_context_mock();
  memset(error_buffer, 0, sizeof(error_buffer));
  memset(&env.context, 0, sizeof(env.context));
}

void setUp(void) { reset_env(); }

void test_setup_context_ok_with_valid_parameters(void) {
  const struct srv_edhoc_parameters params = tst_edhoc_srv_get_default_params();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params, env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_OK, result.status);
}

void test_setup_context_fails_on_invalid_error_buffer(void) {
  const struct com_writable_buffer invalid_buffer = {.bytes = NULL,
                                                     .capacity = 0};
  const struct srv_edhoc_parameters params = tst_edhoc_srv_get_default_params();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params, invalid_buffer);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_INVALID_ERROR_BUFFER,
                    result.status);
}

void test_setup_context_fails_on_null_credentials(void) {
  const struct srv_edhoc_parameters valid_params =
      tst_edhoc_srv_get_default_params();
  const struct srv_edhoc_parameters params = {
      .credentials = NULL,
      .supported_cipher_suites = valid_params.supported_cipher_suites,
      .methods = valid_params.methods};

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params, env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_NULL_CREDENTIALS, result.status);
  tst_edhoc_assert_encoded_error_matches(result.error_buffer,
                                         "Null credentials",
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_setup_context_fails_on_invalid_cipher_suites(void) {
  const struct srv_edhoc_parameters valid_params =
      tst_edhoc_srv_get_default_params();
  const struct srv_edhoc_parameters params = {
      .credentials = valid_params.credentials,
      .supported_cipher_suites = NULL,
      .methods = valid_params.methods};

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params, env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_INVALID_SUPPORTED_SUITES,
                    result.status);
  tst_edhoc_assert_encoded_error_matches(result.error_buffer,
                                         "Invalid supported cipher suites",
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_setup_context_fails_on_invalid_methods(void) {
  const struct srv_edhoc_parameters valid_params =
      tst_edhoc_srv_get_default_params();
  const struct srv_edhoc_methods invalid_methods = {.data = NULL, .size = 0};
  const struct srv_edhoc_parameters params = {
      .credentials = valid_params.credentials,
      .supported_cipher_suites = valid_params.supported_cipher_suites,
      .methods = invalid_methods};

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params, env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_INVALID_METHODS, result.status);
  tst_edhoc_assert_encoded_error_matches(result.error_buffer,
                                         "Invalid EDHOC methods",
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_setup_context_fails_on_psa_crypto_init_failure(void) {
  const struct srv_edhoc_parameters params = tst_edhoc_srv_get_default_params();
  tst_com_set_psa_crypto_init_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params, env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_PSA_INIT, result.status);
  tst_edhoc_assert_encoded_error_matches(result.error_buffer,
                                         "PSA crypto initialization failed",
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_setup_context_fails_on_edhoc_context_init_failure(void) {
  const struct srv_edhoc_parameters params = tst_edhoc_srv_get_default_params();
  tst_com_set_edhoc_context_init_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params, env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_CONTEXT_INIT, result.status);
  tst_edhoc_assert_encoded_error_matches(result.error_buffer,
                                         "EDHOC context initialization failed",
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_setup_context_fails_on_set_methods_failure(void) {
  const struct srv_edhoc_parameters params = tst_edhoc_srv_get_default_params();
  tst_com_set_edhoc_set_methods_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params, env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_SET_METHODS, result.status);
  tst_edhoc_assert_encoded_error_matches(result.error_buffer,
                                         "Failed to set EDHOC methods",
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_setup_context_fails_on_set_cipher_suites_failure(void) {
  const struct srv_edhoc_parameters params = tst_edhoc_srv_get_default_params();
  tst_com_set_edhoc_set_cipher_suites_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params, env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_SET_CIPHER_SUITES, result.status);
  tst_edhoc_assert_encoded_error_matches(result.error_buffer,
                                         "Failed to set cipher suites",
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_setup_context_fails_on_set_connection_id_failure(void) {
  const struct srv_edhoc_parameters params = tst_edhoc_srv_get_default_params();
  tst_com_set_edhoc_set_connection_id_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params, env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_SET_CONNECTION_ID, result.status);
  tst_edhoc_assert_encoded_error_matches(result.error_buffer,
                                         "Failed to set connection ID",
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_setup_context_fails_on_bind_keys_failure(void) {
  const struct srv_edhoc_parameters params = tst_edhoc_srv_get_default_params();
  tst_com_set_edhoc_bind_keys_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params, env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_BIND_KEYS, result.status);
  tst_edhoc_assert_encoded_error_matches(result.error_buffer,
                                         "Failed to bind keys",
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_setup_context_fails_on_bind_crypto_failure(void) {
  const struct srv_edhoc_parameters params = tst_edhoc_srv_get_default_params();
  tst_com_set_edhoc_bind_crypto_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params, env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_BIND_CRYPTO, result.status);
  tst_edhoc_assert_encoded_error_matches(result.error_buffer,
                                         "Failed to bind crypto",
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_setup_context_fails_on_bind_credentials_failure(void) {
  const struct srv_edhoc_parameters params = tst_edhoc_srv_get_default_params();
  tst_com_set_edhoc_bind_credentials_failure();

  const struct com_edhoc_setup_context_result result =
      com_edhoc_setup_context(&env.context, params, env.error_buffer_view);

  TEST_ASSERT_EQUAL(COM_EDHOC_SETUP_CTX_ERR_BIND_CREDENTIALS, result.status);
  tst_edhoc_assert_encoded_error_matches(result.error_buffer,
                                         "Failed to bind credentials",
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}
