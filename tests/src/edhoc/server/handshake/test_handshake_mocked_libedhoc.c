
/**
 * @file test_handshake_mocked_libedhoc.c
 *
 * @brief Unit tests for EDHOC server handshake message processing logic.
 * This file uses stubs to simulate different outcomes from the underlying EDHOC
 * library functions.
 *
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 *
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

#include <coap3/coap.h>
#include <stdlib.h>
#include <unity.h>

#include "edhoc/server/handshake/helpers.h"
#include "edhoc/server/handshake/payload.h"
#include "edhoc/server/handshake/scenarios.h"
#include "edhoc/server/handshake/stubs.h"

static handshake_test_env_t env;

void setUp(void) {
  reset_stub_results();
  memset(&env, 0, sizeof(env));
  setup_testing_environment(&env);
}

// ============================================================================
// Message 1 Handler & Formatting Tests
// ============================================================================

void test_handle_message_1_success(void) {
  const edhoc_server_message_1_result_t result =
      edhoc_server_handle_message_1(&env.request, &env.response);

  TEST_ASSERT_EQUAL(CSH_OK, result.status);
  TEST_ASSERT_NOT_NULL(result.edhoc_ctx);
  free(result.edhoc_ctx);
}

static void ensure_context_is_freed_on_failure(
    const edhoc_server_message_1_result_t result) {
  TEST_ASSERT_NULL(result.edhoc_ctx);
}

void test_handle_message_1_fails_when_prefix_missing(void) {
  const test_edsh_payload_t payload_without_prefix =
      get_invalid_prefix_payload();
  override_test_request_payload(&env, payload_without_prefix.data,
                                payload_without_prefix.length);

  const edhoc_server_message_1_result_t result =
      edhoc_server_handle_message_1(&env.request, &env.response);

  TEST_ASSERT_EQUAL(CSH_ERR_PREFIX_MISSING, result.status);
  ensure_context_is_freed_on_failure(result);
}

void test_handle_message_1_fails_when_setup_context_fails(void) {
  stub_edhoc_setup_res = EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;

  const edhoc_server_message_1_result_t result =
      edhoc_server_handle_message_1(&env.request, &env.response);

  TEST_ASSERT_EQUAL(CSH_ERR_EDHOC_CONTEXT_SETUP_FAILED, result.status);
  ensure_context_is_freed_on_failure(result);
}

static void assert_m1_failed_with_edhoc_error(
    const edhoc_server_message_1_result_t result,
    const edhoc_server_message_1_status_t expected_status) {
  TEST_ASSERT_EQUAL_MESSAGE(expected_status, result.status,
                            "Wrong status code returned");
  ensure_context_is_freed_on_failure(result);
  TEST_ASSERT_EQUAL(MOCK_ERROR_LEN, env.response.payload_length);
  TEST_ASSERT_EQUAL_MEMORY(MOCK_ERROR_PAYLOAD, env.response.payload,
                           MOCK_ERROR_LEN);
}

void test_handle_message_1_fails_on_m1_processing(void) {
  stub_edhoc_process_res = EDHOC_ERROR_CRYPTO_FAILURE;

  const edhoc_server_message_1_result_t result =
      edhoc_server_handle_message_1(&env.request, &env.response);

  assert_m1_failed_with_edhoc_error(result,
                                    CSH_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED);
}

void test_handle_message_1_fails_on_m2_composition(void) {
  stub_edhoc_compose_res = EDHOC_ERROR_BUFFER_TOO_SMALL;

  const edhoc_server_message_1_result_t result =
      edhoc_server_handle_message_1(&env.request, &env.response);

  assert_m1_failed_with_edhoc_error(result,
                                    CSH_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED);
}