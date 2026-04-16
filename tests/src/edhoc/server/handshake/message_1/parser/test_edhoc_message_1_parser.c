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
#include "edhoc/server/handshake/message_1/srv_m1_parser.h"
#include "edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_extract_flow_info.h"

void setUp(void) { tst_srv_edhoc_m1_reset_parse_mock(); }

void test_parser_returns_stripped_message_1_buffer(void) {
  const uint8_t buffer[] = {0x01, 0x02, 0x03, 0x04};
  const size_t buffer_size = sizeof(buffer);
  const struct com_readonly_buffer request_buffer = {
      .bytes = buffer,
      .length = buffer_size,
  };

  const struct srv_edhoc_parse_message_1_result parse_message_1_result =
      srv_edhoc_parse_message_1(request_buffer);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG1_PARSE_OK, parse_message_1_result.status);
  TEST_ASSERT_EQUAL_PTR(&buffer[1],
                        parse_message_1_result.parsed_message_1.bytes);
  TEST_ASSERT_EQUAL(buffer_size - 1,
                    parse_message_1_result.parsed_message_1.length);
}

static void assert_response_empty(
    const struct com_readonly_buffer parsed_message_1) {
  TEST_ASSERT_NULL(parsed_message_1.bytes);
  TEST_ASSERT_EQUAL(0, parsed_message_1.length);
}

void test_parser_fails_on_invalid_request_buffer(void) {
  const struct com_readonly_buffer empty_request = {0};

  const struct srv_edhoc_parse_message_1_result parse_message_1_result =
      srv_edhoc_parse_message_1(empty_request);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG1_PARSE_ERR_INVALID_REQUEST_BUFFER,
                    parse_message_1_result.status);
  assert_response_empty(parse_message_1_result.parsed_message_1);
}

void test_parser_fails_when_prefix_extraction_fails(void) {
  const uint8_t buffer[] = {0x01, 0x02, 0x03, 0x04};
  const size_t buffer_size = sizeof(buffer);
  const struct com_readonly_buffer request_buffer = {
      .bytes = buffer,
      .length = buffer_size,
  };
  tst_srv_edhoc_m1_set_extract_failed();

  const struct srv_edhoc_parse_message_1_result parse_message_1_result =
      srv_edhoc_parse_message_1(request_buffer);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG1_PARSE_ERR_PREFIX_EXTRACTION,
                    parse_message_1_result.status);
  assert_response_empty(parse_message_1_result.parsed_message_1);
}
