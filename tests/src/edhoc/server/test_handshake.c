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