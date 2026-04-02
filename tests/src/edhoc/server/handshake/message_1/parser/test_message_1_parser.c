/**
 * @file test_message_1_parser.c
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

#include "edhoc/server/handshake/message_1/parser.h"
#include "edhoc/server/handshake/message_1/parser/scenarios.h"
#include "edhoc/server/handshake/payload.h"

void test_parser_returns_stripped_message_1_payload(void) {
  const tst_edh_payload_t payload_with_prefix = get_valid_message_1_payload();
  com_request_payload_t parsed_payload = {0};

  const bool parsed = edh_srv_parse_message_1(
      payload_with_prefix.data, payload_with_prefix.length, &parsed_payload);

  TEST_ASSERT_TRUE(parsed);
  TEST_ASSERT_EQUAL_PTR(&payload_with_prefix.data[1], parsed_payload.buffer);
  TEST_ASSERT_EQUAL(payload_with_prefix.length - 1,
                    parsed_payload.length);
}

void test_parser_fails_on_invalid_data(void) {
  const tst_edh_payload_t payload_with_invalid_prefix =
      get_invalid_prefix_payload();
  com_request_payload_t parsed_payload = {0};

  const message_1_parser_test_case_t test_cases[] = {
      {"parse fails with NULL payload", NULL, 5, &parsed_payload},
      {"parse fails with zero length", payload_with_invalid_prefix.data, 0,
       &parsed_payload},
      {"parse fails with NULL output", payload_with_invalid_prefix.data,
       payload_with_invalid_prefix.length, NULL},
  };
  const size_t num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

  for (size_t i = 0; i < num_cases; i++) {
    const bool parsed = edh_srv_parse_message_1(
        test_cases[i].payload, test_cases[i].length, test_cases[i].output);

    TEST_ASSERT_FALSE_MESSAGE(parsed, test_cases[i].description);
  }
}

void test_parser_fails_when_prefix_is_missing(void) {
  const tst_edh_payload_t payload_without_prefix = get_invalid_prefix_payload();
  com_request_payload_t parsed_payload = {0};

  const bool parsed =
      edh_srv_parse_message_1(payload_without_prefix.data,
                              payload_without_prefix.length, &parsed_payload);

  TEST_ASSERT_FALSE(parsed);
  TEST_ASSERT_NULL(parsed_payload.buffer);
  TEST_ASSERT_EQUAL(0, parsed_payload.length);
}