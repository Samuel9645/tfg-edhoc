/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Parser tests for EDHOC server message 1 handling.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

#include <string.h>
#include <unity.h>

#include "common/com_data_models.h"
#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"
#include "edhoc/server/handshake/message_1/srv_m1_parser.h"
#include "edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_extract_flow_info.h"

enum { TST_M1_PARSER_ERROR_BUFFER_SIZE = 256 };

static struct {
  uint8_t error_message[TST_M1_PARSER_ERROR_BUFFER_SIZE];
  struct com_writable_buffer error_buffer_view;
} test_env = {
    .error_buffer_view = {.capacity = TST_M1_PARSER_ERROR_BUFFER_SIZE}};

void setUp(void) {
  tst_srv_edhoc_m1_reset_parse_mock();
  memset(test_env.error_message, 0, TST_M1_PARSER_ERROR_BUFFER_SIZE);
  test_env.error_buffer_view.bytes = test_env.error_message;
}

void test_parser_returns_stripped_message_1_buffer_on_success(void) {
  const uint8_t buffer[] = {0x01, 0x02, 0x03, 0x04};
  const size_t buffer_size = sizeof(buffer);
  const struct com_readonly_buffer request_buffer = {
      .bytes = buffer,
      .length = buffer_size,
  };

  const struct srv_edhoc_parse_message_1_result parse_result =
      srv_edhoc_parse_message_1(request_buffer, test_env.error_buffer_view);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG1_PARSE_OK, parse_result.status);
  TEST_ASSERT_EQUAL_PTR(&buffer[1], parse_result.buffer.bytes);
  TEST_ASSERT_EQUAL(buffer_size - 1, parse_result.buffer.length);
}

void test_parser_fails_and_populates_error_on_empty_request_buffer(void) {
  const struct com_readonly_buffer empty_request = {0};

  const struct srv_edhoc_parse_message_1_result parse_result =
      srv_edhoc_parse_message_1(empty_request, test_env.error_buffer_view);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG1_PARSE_ERR_EMPTY_REQUEST_BUFFER,
                    parse_result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(parse_result.buffer);
}

void test_parser_fails_and_populates_error_when_prefix_extraction_fails(void) {
  const uint8_t buffer[] = {0x01, 0x02, 0x03, 0x04};
  const size_t buffer_size = sizeof(buffer);
  const struct com_readonly_buffer request_buffer = {
      .bytes = buffer,
      .length = buffer_size,
  };
  tst_srv_edhoc_m1_set_extract_failed();

  const struct srv_edhoc_parse_message_1_result parse_result =
      srv_edhoc_parse_message_1(request_buffer, test_env.error_buffer_view);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG1_PARSE_ERR_PREFIX_EXTRACTION,
                    parse_result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(parse_result.buffer);
}

void test_parser_fails_on_invalid_error_buffer(void) {
  const uint8_t buffer[] = {0x01, 0x02, 0x03, 0x04};
  const struct com_readonly_buffer request_buffer = {
      .bytes = buffer,
      .length = sizeof(buffer),
  };
  const struct com_writable_buffer invalid_error_buffer = {.bytes = NULL,
                                                           .capacity = 0};

  const struct srv_edhoc_parse_message_1_result parse_result =
      srv_edhoc_parse_message_1(request_buffer, invalid_error_buffer);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG1_PARSE_ERR_INVALID_ERROR_BUFFER,
                    parse_result.status);
  TEST_ASSERT_NULL(parse_result.buffer.bytes);
  TEST_ASSERT_EQUAL(0, parse_result.buffer.length);
}
