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
#include "edhoc/server/handshake/helpers.h"
#include "edhoc/server/handshake/scenarios.h"

void test_remove_prefix_success(void) {
  enum { FIRST_BYTE = 0x01 };
  const uint8_t payload_with_prefix[] = {EDHOC_CC_CBOR_TRUE, FIRST_BYTE};
  const size_t original_len = sizeof(payload_with_prefix);
  const uint8_t* payload_ptr = payload_with_prefix;
  size_t payload_len = original_len;

  edhoc_server_handshake_error result =
      edhoc_server_remove_cbor_true_prefix(&payload_ptr, &payload_len);

  TEST_ASSERT_EQUAL(CSH_OK, result);
  TEST_ASSERT_EQUAL(FIRST_BYTE, *payload_ptr);
  TEST_ASSERT_EQUAL_PTR(&payload_with_prefix[1], payload_ptr);
  TEST_ASSERT_EQUAL(original_len - 1, payload_len);
}

void test_remove_prefix_fails_on_missing_prefix(void) {
  const uint8_t payload_without_prefix[] = {0x00, 0x02, 0x03};
  const size_t original_len = sizeof(payload_without_prefix);
  const uint8_t* payload_ptr = payload_without_prefix;
  size_t payload_len = original_len;

  edhoc_server_handshake_error result =
      edhoc_server_remove_cbor_true_prefix(&payload_ptr, &payload_len);

  TEST_ASSERT_EQUAL(CSH_ERR_PREFIX_MISSING, result);
  TEST_ASSERT_EQUAL_PTR(payload_without_prefix, payload_ptr);
  TEST_ASSERT_EQUAL(original_len, payload_len);
}

void test_remove_prefix_fails_on_invalid_payload(void) {
  const uint8_t* null_payload_ptr = NULL;
  size_t payload_len = 0;

  edhoc_server_handshake_error result =
      edhoc_server_remove_cbor_true_prefix(&null_payload_ptr, &payload_len);

  TEST_ASSERT_EQUAL(CSH_ERR_INVALID_PAYLOAD, result);
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
    coap_pdu_code_t result = edhoc_server_handle_message_1(
        test_cases[i].request, test_cases[i].response);
    reset_test_response(&env.response);
    TEST_ASSERT_EQUAL_MESSAGE(COAP_RESPONSE_CODE_BAD_REQUEST, result,
                              test_cases[i].description);
    assert_response_untouched(test_cases[i].response);
  }
}
