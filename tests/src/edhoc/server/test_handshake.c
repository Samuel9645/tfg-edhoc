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
#include "edhoc/server/handshake.h"

void test_remove_cbor_true_prefix_removes_prefix_when_present(void) {
  enum { FIRST_BYTE = 0x01 };
  const uint8_t payload_with_prefix[] = {EDHOC_CC_CBOR_TRUE, FIRST_BYTE};
  const size_t original_len = sizeof(payload_with_prefix);
  const uint8_t* payload_ptr = payload_with_prefix;
  size_t payload_len = original_len;

  edhoc_server_handshake_error result =
      edhoc_server_remove_cbor_true_prefix(&payload_ptr, &payload_len);

  TEST_ASSERT_EQUAL(CSH_OK, result);
  TEST_ASSERT_EQUAL(FIRST_BYTE, *payload_ptr);
  TEST_ASSERT_EQUAL(original_len - 1, payload_len);
}

void test_remove_cbor_true_prefix_returns_error_when_prefix_missing(void) {
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

void test_remove_cbor_true_prefix_returns_error_on_invalid_payload(void) {
  const uint8_t* null_payload_ptr = NULL;
  size_t payload_len = 0;

  edhoc_server_handshake_error result =
      edhoc_server_remove_cbor_true_prefix(&null_payload_ptr, &payload_len);

  TEST_ASSERT_EQUAL(CSH_ERR_INVALID_PAYLOAD, result);
  TEST_ASSERT_NULL(null_payload_ptr);
  TEST_ASSERT_EQUAL(0, payload_len);
}