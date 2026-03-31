/**
 * @file test_handshake.c
 *
 * @brief Unit tests for EDHOC server handshake message processing logic.
 *
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 *
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

#include <coap3/coap.h>
#include <string.h>
#include <unity.h>

#include "edhoc/common/constants.h"
#include "edhoc/edhoc_config.h"
#include "edhoc/server/handshake/helpers.h"
#include "edhoc/server/handshake/payload.h"
#include "edhoc/server/handshake/scenarios.h"

void test_remove_prefix_success(void) {
  test_edsh_payload_t payload_with_prefix = get_valid_message_1_payload();
  const size_t original_len = payload_with_prefix.length;
  const uint8_t* payload_ptr = payload_with_prefix.data;
  size_t payload_len = original_len;

  edhoc_server_handshake_status_t result =
      edhoc_server_remove_cbor_true_prefix(&payload_ptr, &payload_len);

  TEST_ASSERT_EQUAL(CSH_OK, result);
  TEST_ASSERT_EQUAL_PTR(&payload_with_prefix.data[1], payload_ptr);
  TEST_ASSERT_EQUAL(original_len - 1, payload_len);
}

void test_remove_prefix_fails_on_missing_prefix(void) {
  test_edsh_payload_t payload_without_prefix = get_invalid_prefix_payload();
  const size_t original_len = payload_without_prefix.length;
  const uint8_t* payload_ptr = payload_without_prefix.data;
  size_t payload_len = original_len;

  edhoc_server_handshake_status_t result =
      edhoc_server_remove_cbor_true_prefix(&payload_ptr, &payload_len);

  TEST_ASSERT_EQUAL(CSH_ERR_PREFIX_MISSING, result);
  TEST_ASSERT_EQUAL_PTR(payload_without_prefix.data, payload_ptr);
  TEST_ASSERT_EQUAL(original_len, payload_len);
}

void test_remove_prefix_fails_on_invalid_payload(void) {
  const uint8_t* null_payload_ptr = NULL;
  size_t payload_len = 0;

  edhoc_server_handshake_status_t result =
      edhoc_server_remove_cbor_true_prefix(&null_payload_ptr, &payload_len);

  TEST_ASSERT_EQUAL(CSH_ERR_INVALID_ARGS, result);
  TEST_ASSERT_NULL(null_payload_ptr);
  TEST_ASSERT_EQUAL(0, payload_len);
}

void test_handle_message_1_fails_on_invalid_data(void) {
  handshake_test_env_t env = {0};
  setup_testing_environment(&env);
  edhoc_server_common_request_data_t empty_request = {0};
  common_response_buffer_t empty_response = {0};

  handshake_test_case_t test_cases[] = {
      {"Both arguments NULL", NULL, NULL},
      {"Request is NULL", NULL, &env.response},
      {"Response is NULL", &env.request, NULL},
      {"Empty request data", &empty_request, &env.response},
      {"Empty response data", &env.request, &empty_response},
  };

  size_t num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < num_cases; i++) {
    edhoc_server_handshake_status_t result = edhoc_server_handle_message_1(
        test_cases[i].request, test_cases[i].response);
    reset_test_response(&env.response);
    TEST_ASSERT_EQUAL_MESSAGE(CSH_ERR_INVALID_ARGS, result,
                              test_cases[i].description);
    assert_response_untouched(test_cases[i].response);
  }
}

void test_handle_message_1_fails_on_too_large_request_data(void) {
  handshake_test_env_t env = {0};
  setup_testing_environment(&env);
  uint8_t large_buffer[EDC_MESSAGE_BUFFER_LENGTH + 1] = {0};
  env.request.request_data.payload = large_buffer;
  env.request.request_data.payload_length = sizeof(large_buffer);

  edhoc_server_handshake_status_t result =
      edhoc_server_handle_message_1(&env.request, &env.response);

  TEST_ASSERT_EQUAL(CSH_ERR_PAYLOAD_TOO_LARGE, result);
  assert_response_untouched(&env.response);
}
