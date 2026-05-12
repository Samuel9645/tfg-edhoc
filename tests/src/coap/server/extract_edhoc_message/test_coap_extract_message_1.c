/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Tests for the extract of the CoAP requests for EDHOC server message 1.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [RFC 9528
 * A.2](https://datatracker.ietf.org/doc/html/rfc9528#name-transferring-edhoc-over-coa)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

#include <string.h>
#include <unity.h>

#include "coap/server/extract_edhoc_message/srv_coap_extract_m1.h"
#include "common/com_data_models.h"
#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"
#include "edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_extract_flow_info.h"

enum { TST_M1_EXTRACT_ERROR_BUFFER_SIZE = 256 };

static struct {
  uint8_t error_message[TST_M1_EXTRACT_ERROR_BUFFER_SIZE];
  struct com_writable_buffer error_buffer_view;
} test_env = {
    .error_buffer_view = {.capacity = TST_M1_EXTRACT_ERROR_BUFFER_SIZE}};

void setUp(void) {
  tst_srv_edhoc_m1_reset_extract_mock();
  memset(test_env.error_message, 0, TST_M1_EXTRACT_ERROR_BUFFER_SIZE);
  test_env.error_buffer_view.bytes = test_env.error_message;
}

void test_extract_returns_stripped_message_1_buffer_on_success(void) {
  enum { CBOR_TRUE = 0xf5 };

  const uint8_t buffer[] = {CBOR_TRUE, 0x02, 0x03, 0x04};
  const size_t buffer_size = sizeof(buffer);
  const struct com_readonly_buffer request_buffer = {
      .bytes = buffer,
      .length = buffer_size,
  };

  const struct srv_coap_extract_message_1_result parse_result =
      srv_coap_extract_message_1(request_buffer, test_env.error_buffer_view);

  TEST_ASSERT_EQUAL(SRV_COAP_EXTRACT_MSG1_OK, parse_result.status);
  TEST_ASSERT_EQUAL_PTR(&buffer[1], parse_result.buffer.bytes);
  TEST_ASSERT_EQUAL(buffer_size - 1, parse_result.buffer.length);
}

void test_extract_fails_when_message_is_from_unsupported_reverse_flow(void) {
  // Extracted from RFC trace
  const uint8_t MESSAGE_1_SUITE_0_METHOD_0[] = {
      0x00, 0x00, 0x58, 0x20, 0x31, 0xf8, 0x2c, 0x7b, 0x5b, 0x9c,
      0xbb, 0xf0, 0xf1, 0x94, 0xd9, 0x13, 0xcc, 0x12, 0xef, 0x15,
      0x32, 0xd3, 0x28, 0xef, 0x32, 0x63, 0x2a, 0x48, 0x81, 0xa1,
      0xc0, 0x70, 0x1e, 0x23, 0x7f, 0x04, 0x2d};
  const struct com_readonly_buffer request_buffer = {
      .bytes = MESSAGE_1_SUITE_0_METHOD_0,
      .length = sizeof(MESSAGE_1_SUITE_0_METHOD_0)};

  const struct srv_coap_extract_message_1_result parse_result =
      srv_coap_extract_message_1(request_buffer, test_env.error_buffer_view);

  TEST_ASSERT_EQUAL(SRV_COAP_EXTRACT_MSG1_ERR_UNSUPPORTED_FORWARD_FLOW,
                    parse_result.status);
  tst_edhoc_assert_encoded_error_matches(
      parse_result.buffer, "Message 1 Extract error: Unsupported forward flow",
      EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_extract_fails_and_populates_error_on_empty_request_buffer(void) {
  const struct com_readonly_buffer empty_request = {0};

  const struct srv_coap_extract_message_1_result parse_result =
      srv_coap_extract_message_1(empty_request, test_env.error_buffer_view);

  TEST_ASSERT_EQUAL(SRV_COAP_EXTRACT_MSG1_ERR_EMPTY_REQUEST_BUFFER,
                    parse_result.status);
  tst_edhoc_assert_encoded_error_matches(
      parse_result.buffer, "Message 1 Extract error: Empty request buffer",
      EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_extract_fails_and_populates_error_when_prefix_extraction_fails(void) {
  const uint8_t buffer[] = {0x01, 0x02, 0x03, 0x04};
  const size_t buffer_size = sizeof(buffer);
  const struct com_readonly_buffer request_buffer = {
      .bytes = buffer,
      .length = buffer_size,
  };
  tst_srv_edhoc_m1_set_extract_failed();

  const struct srv_coap_extract_message_1_result parse_result =
      srv_coap_extract_message_1(request_buffer, test_env.error_buffer_view);

  TEST_ASSERT_EQUAL(SRV_COAP_EXTRACT_MSG1_ERR_PREFIX_EXTRACTION,
                    parse_result.status);
  tst_edhoc_assert_encoded_error_matches(
      parse_result.buffer, "Message 1 Extract error: Prefix extraction failed",
      EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_extract_fails_on_invalid_error_buffer(void) {
  const uint8_t buffer[] = {0x01, 0x02, 0x03, 0x04};
  const struct com_readonly_buffer request_buffer = {
      .bytes = buffer,
      .length = sizeof(buffer),
  };
  const struct com_writable_buffer invalid_error_buffer = {.bytes = NULL,
                                                           .capacity = 0};

  const struct srv_coap_extract_message_1_result parse_result =
      srv_coap_extract_message_1(request_buffer, invalid_error_buffer);

  TEST_ASSERT_EQUAL(SRV_COAP_EXTRACT_MSG1_ERR_INVALID_ERROR_BUFFER,
                    parse_result.status);
  TEST_ASSERT_NULL(parse_result.buffer.bytes);
  TEST_ASSERT_EQUAL(0, parse_result.buffer.length);
}
