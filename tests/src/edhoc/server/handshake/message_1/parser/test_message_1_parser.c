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

#include "edhoc/server/handshake/message_1/parser/tst_m1_parser_scenarios.h"
#include "edhoc/server/handshake/message_1/srv_m1_parser.h"
#include "edhoc/server/handshake/tst_srv_payload.h"

void test_parser_returns_stripped_message_1_payload(void) {
  const tst_edh_payload_t payload_with_prefix = get_valid_message_1_payload();
  const com_readonly_buffer_t request_buffer = {
      .bytes = payload_with_prefix.data,
      .length = payload_with_prefix.length,
  };

  const edh_srv_parse_message_1_result_t parse_message_1_result =
      edh_srv_parse_message_1(request_buffer);

  TEST_ASSERT_EQUAL(EDH_SRV_MSG1_PARSE_OK, parse_message_1_result.status);
  TEST_ASSERT_EQUAL_PTR(&payload_with_prefix.data[1],
                        parse_message_1_result.parsed_message_1.bytes);
  TEST_ASSERT_EQUAL(payload_with_prefix.length - 1,
                    parse_message_1_result.parsed_message_1.length);
}

void test_parser_fails_on_invalid_request_buffer(void) {
  const com_readonly_buffer_t empty_request = {0};

  const edh_srv_parse_message_1_result_t parse_message_1_result =
      edh_srv_parse_message_1(empty_request);

  TEST_ASSERT_EQUAL(EDH_SRV_MSG1_PARSE_ERR_INVALID_REQUEST_BUFFER,
                    parse_message_1_result.status);
  TEST_ASSERT_NULL(parse_message_1_result.parsed_message_1.bytes);
  TEST_ASSERT_EQUAL(0, parse_message_1_result.parsed_message_1.length);
}

void test_parser_fails_when_prefix_is_missing(void) {
  const tst_edh_payload_t payload_without_prefix = get_invalid_prefix_payload();
  const com_readonly_buffer_t missing_prefix_request = {
      .bytes = payload_without_prefix.data,
      .length = payload_without_prefix.length,
  };

  const edh_srv_parse_message_1_result_t parse_message_1_result =
      edh_srv_parse_message_1(missing_prefix_request);

  TEST_ASSERT_EQUAL(EDH_SRV_MSG1_PARSE_ERR_PREFIX_MISSING,
                    parse_message_1_result.status);
  TEST_ASSERT_NULL(parse_message_1_result.parsed_message_1.bytes);
  TEST_ASSERT_EQUAL(0, parse_message_1_result.parsed_message_1.length);
}