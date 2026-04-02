
/**
 * @file test_message_1_handler_mocked_libedhoc.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Unit tests for EDHOC server handshake message processing logic.
 * This file uses stubs to simulate different outcomes from the underlying EDHOC
 * library functions.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

#include <coap3/coap.h>
#include <edhoc.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "edhoc/server/handshake/message_1_handler/helpers.h"
#include "edhoc/server/handshake/message_1_handler/stubs.h"
#include "edhoc/server/handshake/payload.h"

static tst_edh_srv_hnd_env_t env;

void setUp(void) {
  tst_edh_srv_hnd_reset_stub_results();
  memset(&env, 0, sizeof(env));
  tst_edh_srv_hnd_setup_env(&env);
}

// ============================================================================
// Message 1 Handler & Formatting Tests
// ============================================================================

void test_handle_message_1_success(void) {
  const ehd_message_1_handler_result_t result =
      edh_srv_handle_message_1(&env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG1_HDL_OK, result.status);
  TEST_ASSERT_NOT_NULL(result.edhoc_ctx);
  free(result.edhoc_ctx);
}

static void ensure_context_is_freed_on_failure(
    const ehd_message_1_handler_result_t result) {
  TEST_ASSERT_NULL(result.edhoc_ctx);
}

void test_handle_message_1_fails_when_prefix_missing(void) {
  const tst_edh_srv_hnd_payload_t payload_without_prefix =
      get_invalid_prefix_payload();
  tst_edh_srv_hnd_override_req(&env, payload_without_prefix.data,
                               payload_without_prefix.length);

  const ehd_message_1_handler_result_t result =
      edh_srv_handle_message_1(&env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG1_HDL_ERR_PREFIX_MISSING, result.status);
  ensure_context_is_freed_on_failure(result);
}

void test_handle_message_1_fails_when_setup_context_fails(void) {
  tst_edh_srv_hnd_stub_edhoc_setup_res = EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;

  const ehd_message_1_handler_result_t result =
      edh_srv_handle_message_1(&env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG1_HDL_ERR_EDHOC_CONTEXT_SETUP_FAILED, result.status);
  ensure_context_is_freed_on_failure(result);
}

static void assert_m1_failed_with_edhoc_error(
    const ehd_message_1_handler_result_t result,
    const edh_message_1_handler_status_t expected_status) {
  TEST_ASSERT_EQUAL_MESSAGE(expected_status, result.status,
                            "Wrong status code returned");
  ensure_context_is_freed_on_failure(result);
  TEST_ASSERT_EQUAL(TST_EDH_SRV_HND_MOCK_ERROR_LEN,
                    env.response.payload_length);
  TEST_ASSERT_EQUAL_MEMORY(TST_EDH_SRV_HND_MOCK_ERROR_PAYLOAD,
                           env.response.payload,
                           TST_EDH_SRV_HND_MOCK_ERROR_LEN);
}

void test_handle_message_1_fails_on_m1_processing(void) {
  tst_edh_srv_hnd_stub_edhoc_process_res = EDHOC_ERROR_CRYPTO_FAILURE;

  const ehd_message_1_handler_result_t result =
      edh_srv_handle_message_1(&env.request, &env.response);

  assert_m1_failed_with_edhoc_error(
      result, EDH_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED);
}

void test_handle_message_1_fails_on_m2_composition(void) {
  tst_edh_srv_hnd_stub_edhoc_compose_res = EDHOC_ERROR_BUFFER_TOO_SMALL;

  const ehd_message_1_handler_result_t result =
      edh_srv_handle_message_1(&env.request, &env.response);

  assert_m1_failed_with_edhoc_error(
      result, EDH_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED);
}

void test_handle_message_1_propagates_library_error_payload(void) {
  tst_edh_srv_hnd_stub_edhoc_process_res =
      EDHOC_ERROR_CODE_WRONG_SELECTED_CIPHER_SUITE;
  const uint8_t expected_error_pdu[] = {0x01, 0x02, 0x03};
  tst_edh_srv_hnd_set_stub_error_response(expected_error_pdu,
                                          sizeof(expected_error_pdu));

  const ehd_message_1_handler_result_t result =
      edh_srv_handle_message_1(&env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED,
                    result.status);
  TEST_ASSERT_EQUAL(sizeof(expected_error_pdu), env.response.payload_length);
  TEST_ASSERT_EQUAL_MEMORY(expected_error_pdu, env.response.payload,
                           sizeof(expected_error_pdu));
}