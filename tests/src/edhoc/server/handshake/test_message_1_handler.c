/**
 * @file test_message_1_handler.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Unit tests for EDHOC server handshake message processing logic.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

#include <coap3/coap.h>
#include <string.h>
#include <unity.h>

#include "edhoc/config.h"
#include "edhoc/server/handshake/helpers.h"
#include "edhoc/server/handshake/payload.h"
#include "edhoc/server/handshake/scenarios.h"

// ============================================================================
// Message 1 Handler & Formatting Tests
// ============================================================================

void test_is_properly_formatted_message_1_success(void) {
  const tst_edh_srv_hnd_payload_t payload_with_prefix =
      get_valid_message_1_payload();
  const uint8_t* payload_ptr = payload_with_prefix.data;
  TEST_ASSERT_TRUE(edh_srv_hnd_m1_is_properly_formatted(
      payload_with_prefix.data, payload_with_prefix.length));
  TEST_ASSERT_EQUAL_PTR(payload_ptr, payload_with_prefix.data);
}

void test_is_properly_formatted_message_1_fails_on_invalid_payload(void) {
  const tst_edh_srv_hnd_payload_t payload_without_prefix =
      get_invalid_prefix_payload();
  const uint8_t* payload_ptr = payload_without_prefix.data;

  TEST_ASSERT_FALSE(edh_srv_hnd_m1_is_properly_formatted(NULL, 5));
  TEST_ASSERT_FALSE(
      edh_srv_hnd_m1_is_properly_formatted(payload_without_prefix.data, 0));
  TEST_ASSERT_FALSE(edh_srv_hnd_m1_is_properly_formatted(
      payload_without_prefix.data, payload_without_prefix.length));
  TEST_ASSERT_EQUAL_PTR(payload_ptr, payload_without_prefix.data);
}

void test_remove_prefix_success(void) {
  tst_edh_srv_hnd_payload_t payload_with_prefix = get_valid_message_1_payload();
  const size_t original_len = payload_with_prefix.length;
  const uint8_t* payload_ptr = payload_with_prefix.data;
  size_t payload_len = original_len;

  const enum edh_srv_hnd_m1_status result =
      edh_srv_hnd_m1_remove_cbor_true_prefix(&payload_ptr, &payload_len);

  TEST_ASSERT_EQUAL(EDH_SERV_HND_M1_OK, result);
  TEST_ASSERT_EQUAL_PTR(&payload_with_prefix.data[1], payload_ptr);
  TEST_ASSERT_EQUAL(original_len - 1, payload_len);
}

void test_remove_prefix_fails_on_missing_prefix(void) {
  const tst_edh_srv_hnd_payload_t payload_without_prefix =
      get_invalid_prefix_payload();
  const size_t original_len = payload_without_prefix.length;
  const uint8_t* payload_ptr = payload_without_prefix.data;
  size_t payload_len = original_len;

  const enum edh_srv_hnd_m1_status result =
      edh_srv_hnd_m1_remove_cbor_true_prefix(&payload_ptr, &payload_len);

  TEST_ASSERT_EQUAL(EDH_SERV_HND_M1_ERR_PREFIX_MISSING, result);
  TEST_ASSERT_EQUAL_PTR(payload_without_prefix.data, payload_ptr);
  TEST_ASSERT_EQUAL(original_len, payload_len);
}

void test_remove_prefix_fails_on_invalid_payload(void) {
  const uint8_t* null_payload_ptr = NULL;
  size_t payload_len = 0;

  const enum edh_srv_hnd_m1_status result =
      edh_srv_hnd_m1_remove_cbor_true_prefix(&null_payload_ptr, &payload_len);

  TEST_ASSERT_EQUAL(EDH_SERV_HND_M1_ERR_INVALID_ARGS, result);
  TEST_ASSERT_NULL(null_payload_ptr);
  TEST_ASSERT_EQUAL(0, payload_len);
}

void test_handle_message_1_fails_on_invalid_data(void) {
  tst_edh_srv_hnd_env_t env = {0};
  tst_edh_srv_hnd_setup_env(&env);
  edh_srv_hnd_m1_request_t empty_request = {0};
  com_response_buffer_t empty_response = {0};

  const handshake_test_case_t test_cases[] = {
      {"Both arguments NULL", NULL, NULL},
      {"Request is NULL", NULL, &env.response},
      {"Response is NULL", &env.request, NULL},
      {"Empty request data", &empty_request, &env.response},
      {"Empty response data", &env.request, &empty_response},
  };

  const size_t num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < num_cases; i++) {
    const edh_srv_hnd_m1_result_t result =
        edh_srv_hnd_m1_handle(test_cases[i].request, test_cases[i].response);
    tst_edh_srv_hnd_reset_res(&env.response);
    TEST_ASSERT_EQUAL_MESSAGE(EDH_SERV_HND_M1_ERR_INVALID_ARGS, result.status,
                              test_cases[i].description);
    TEST_ASSERT_NULL(result.edhoc_ctx);
    tst_edh_srv_hnd_assert_res_clean(test_cases[i].response);
  }
}

void test_handle_message_1_fails_on_too_large_request_data(void) {
  tst_edh_srv_hnd_env_t env = {0};
  tst_edh_srv_hnd_setup_env(&env);
  const uint8_t large_buffer[EDH_CFG_MESSAGE_BUFFER_LENGTH + 1] = {0};
  tst_edh_srv_hnd_override_req(&env, large_buffer, sizeof(large_buffer));

  const edh_srv_hnd_m1_result_t result =
      edh_srv_hnd_m1_handle(&env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_SERV_HND_M1_ERR_PAYLOAD_TOO_LARGE, result.status);
  TEST_ASSERT_NULL(result.edhoc_ctx);
  tst_edh_srv_hnd_assert_res_clean(&env.response);
}