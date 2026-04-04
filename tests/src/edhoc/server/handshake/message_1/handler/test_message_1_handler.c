
/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Handler tests for EDHOC server message 1.
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

#include "edhoc/config.h"
#include "edhoc/server/handshake/common/tst_srv_m1_helpers.h"
#include "edhoc/server/handshake/message_1/handler/tst_m1_hdl_helpers.h"
#include "edhoc/server/handshake/message_1/handler/tst_m1_hdl_scenarios.h"
#include "edhoc/server/handshake/message_1/handler/tst_srv_m1_stubs.h"

static tst_message_1_handler_env_t env = {0};

void setUp(void) {
  tst_edh_srv_hnd_reset_stub_results();
  memset(&env, 0, sizeof(env));
  tst_edh_setup_message_1_handler_env(&env);
}

// ============================================================================
// Message 1 Handler Tests
// ============================================================================

void test_handler_ok_for_valid_data(void) {
  const ehd_srv_message_1_handler_result_t result =
      edh_srv_handle_message_1(&env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_SRV_MSG1_HDL_OK, result.status);
  TEST_ASSERT_NOT_NULL(result.edhoc_ctx);
  free(result.edhoc_ctx);
}

static void ensure_context_is_freed_on_failure(
    const ehd_srv_message_1_handler_result_t result) {
  TEST_ASSERT_NULL(result.edhoc_ctx);
}

void test_handler_fails_on_invalid_data(void) {
  edh_srv_message_1_request_t empty_request = {0};
  com_writable_buffer_t empty_response = tst_edh_invalid_response();

  const tst_edh_srv_hnd_test_case_message_1_handler_t test_cases[] = {
      {"both arguments are missing", NULL, NULL},
      {"request is missing", NULL, &env.response},
      {"response is missing", &env.request, NULL},
      {"request data is empty", &empty_request, &env.response},
      {"response buffer is empty", &env.request, &empty_response},
  };

  const size_t num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < num_cases; i++) {
    const ehd_srv_message_1_handler_result_t result =
        edh_srv_handle_message_1(test_cases[i].request, test_cases[i].response);

    TEST_ASSERT_EQUAL_MESSAGE(EDH_SRV_MSG1_HDL_ERR_INVALID_ARGS, result.status,
                              test_cases[i].description);
    ensure_context_is_freed_on_failure(result);
    if (test_cases[i].response) {
      tst_edh_srv_assert_response_clean(test_cases[i].response);
    }
  }
}

void test_handler_fails_on_too_large_request_data(void) {
  static const uint8_t large_buffer[EDH_CFG_MESSAGE_BUFFER_LENGTH + 1] = {0};
  env.request.payload.bytes = large_buffer;
  env.request.payload.length = sizeof(large_buffer);

  const ehd_srv_message_1_handler_result_t result =
      edh_srv_handle_message_1(&env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_SRV_MSG1_HDL_ERR_PAYLOAD_TOO_LARGE, result.status);
  ensure_context_is_freed_on_failure(result);
}

void test_handler_fails_when_context_setup_fails(void) {
  tst_edh_srv_hnd_stub_edhoc_setup_res = EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;

  const ehd_srv_message_1_handler_result_t result =
      edh_srv_handle_message_1(&env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_SRV_MSG1_HDL_ERR_EDHOC_CONTEXT_SETUP_FAILED, result.status);
  ensure_context_is_freed_on_failure(result);
}

static void assert_m1_failed_with_edhoc_error(
    const ehd_srv_message_1_handler_result_t result,
    const edh_srv_message_1_handler_status_t expected_status) {
  TEST_ASSERT_EQUAL_MESSAGE(expected_status, result.status,
                            "Wrong status code returned");
  ensure_context_is_freed_on_failure(result);
  TEST_ASSERT_EQUAL(TST_EDH_SRV_HND_MOCK_ERROR_LEN, env.response.length);
  TEST_ASSERT_EQUAL_MEMORY(TST_EDH_SRV_HND_MOCK_ERROR_PAYLOAD,
                           env.response.bytes, TST_EDH_SRV_HND_MOCK_ERROR_LEN);
}

void test_handler_fails_when_message_1_processing_fails(void) {
  tst_edh_srv_hnd_stub_edhoc_process_res = EDHOC_ERROR_CRYPTO_FAILURE;

  const ehd_srv_message_1_handler_result_t result =
      edh_srv_handle_message_1(&env.request, &env.response);

  assert_m1_failed_with_edhoc_error(
      result, EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED);
}

void test_handler_fails_when_message_2_composition_fails(void) {
  tst_edh_srv_hnd_stub_edhoc_compose_res = EDHOC_ERROR_BUFFER_TOO_SMALL;

  const ehd_srv_message_1_handler_result_t result =
      edh_srv_handle_message_1(&env.request, &env.response);

  assert_m1_failed_with_edhoc_error(
      result, EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED);
}

void test_handler_propagates_library_error_payload(void) {
  tst_edh_srv_hnd_stub_edhoc_process_res =
      EDHOC_ERROR_CODE_WRONG_SELECTED_CIPHER_SUITE;
  const uint8_t expected_error_pdu[] = {0x81, 0x82, 0x77};
  tst_edh_srv_hnd_set_stub_error_response(expected_error_pdu,
                                          sizeof(expected_error_pdu));

  const ehd_srv_message_1_handler_result_t result =
      edh_srv_handle_message_1(&env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED,
                    result.status);
  TEST_ASSERT_EQUAL(sizeof(expected_error_pdu), env.response.length);
  TEST_ASSERT_EQUAL_MEMORY(expected_error_pdu, env.response.bytes,
                           sizeof(expected_error_pdu));
}