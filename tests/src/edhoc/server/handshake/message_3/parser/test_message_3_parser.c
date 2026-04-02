/**
 * @file test_message_3_parser.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Parser tests for EDHOC server message 3 handling.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

#include <edhoc.h>
#include <string.h>
#include <unity.h>

#include "edhoc/server/handshake/message_3/parser.h"
#include "edhoc/server/handshake/message_3/parser/scenarios.h"
#include "edhoc/server/handshake/message_3/parser/stubs.h"

enum { TEST_MESSAGE_3_PAYLOAD_LENGTH = 5 };

static const uint8_t
    DEFAULT_TEST_MESSAGE_3_PAYLOAD[TEST_MESSAGE_3_PAYLOAD_LENGTH] = {
        0x01, 0x02, 0x03, 0x04, 0x05};
static uint8_t request_payload[TEST_MESSAGE_3_PAYLOAD_LENGTH] = {0};
static struct edhoc_context context = {0};
static struct edhoc_extracted_fields extracted_fields = {0};

void setUp(void) {
  tst_edh_srv_message_3_parser_reset_stub_results();
  memcpy(request_payload, DEFAULT_TEST_MESSAGE_3_PAYLOAD,
         sizeof(request_payload));
  context = (struct edhoc_context){0};
  extracted_fields = (struct edhoc_extracted_fields){0};
}

void test_parser_returns_extracted_fields_for_valid_message_3_data(void) {
  const bool parsed = edh_srv_parse_message_3(
      request_payload, sizeof(request_payload), &context, &extracted_fields);

  TEST_ASSERT_TRUE(parsed);
  TEST_ASSERT_EQUAL_PTR(request_payload, extracted_fields.buffer);
  TEST_ASSERT_EQUAL(sizeof(request_payload), extracted_fields.buffer_size);
  TEST_ASSERT_EQUAL_PTR(request_payload, extracted_fields.edhoc_message_ptr);
  TEST_ASSERT_EQUAL(sizeof(request_payload),
                    extracted_fields.edhoc_message_size);
}

void test_parser_fails_on_invalid_data(void) {
  const message_3_parser_test_case_t test_cases[] = {
      {"request payload is missing", NULL, sizeof(request_payload), &context,
       &extracted_fields},
      {"request payload length is zero", request_payload, 0, &context,
       &extracted_fields},
      {"context is missing", request_payload, sizeof(request_payload), NULL,
       &extracted_fields},
      {"extracted fields output is missing", request_payload,
       sizeof(request_payload), &context, NULL},
  };

  const size_t num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < num_cases; i++) {
    const bool parsed = edh_srv_parse_message_3(
        test_cases[i].request_payload, test_cases[i].request_length,
        test_cases[i].context, test_cases[i].extracted_fields);

    TEST_ASSERT_FALSE_MESSAGE(parsed, test_cases[i].description);
  }
}

void test_parser_fails_when_connection_id_extraction_fails(void) {
  tst_edh_srv_message_3_parser_stub_extract_result =
      EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;

  const bool parsed = edh_srv_parse_message_3(
      request_payload, sizeof(request_payload), &context, &extracted_fields);

  TEST_ASSERT_FALSE(parsed);
}

void test_parser_fails_when_connection_id_does_not_match_context(void) {
  tst_edh_srv_message_3_parser_stub_equal_result = false;

  const bool parsed = edh_srv_parse_message_3(
      request_payload, sizeof(request_payload), &context, &extracted_fields);

  TEST_ASSERT_FALSE(parsed);
}
