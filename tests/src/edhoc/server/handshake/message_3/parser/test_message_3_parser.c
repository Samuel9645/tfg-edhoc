/**
 * @file
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

#include "edhoc/server/handshake/message_3/parser/tst_srv_mock_m3_parser_deps.h"
#include "edhoc/server/handshake/message_3/srv_m3_parser.h"

enum { TEST_MSG_3_PAYLOAD_LENGTH = 5 };

static const struct {
  const uint8_t request_buffer[TEST_MSG_3_PAYLOAD_LENGTH];
  const struct com_readonly_buffer valid_request;
  struct edhoc_context context;
} env = {
    .request_buffer = {0x01, 0x02, 0x03, 0x04, 0x05},
    .valid_request = {.bytes = env.request_buffer,
                      .length = TEST_MSG_3_PAYLOAD_LENGTH},
};

void setUp(void) { tst_srv_edhoc_message_3_parser_reset_stub_results(); }

void test_parser_advances_pointers_on_success(void) {
  const uint8_t* advanced_pointed = env.request_buffer + 2;
  const int reduced_length = TEST_MSG_3_PAYLOAD_LENGTH - 2;
  tst_srv_m3_parser_set_success_data(
      (struct edhoc_extracted_fields){.edhoc_message_ptr = advanced_pointed,
                                      .edhoc_message_size = reduced_length});

  const struct srv_edhoc_parse_message_3_result result =
      srv_edhoc_parse_message_3(env.valid_request, &env.context);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_PARSE_OK, result.status);
  TEST_ASSERT_EQUAL_PTR(advanced_pointed, result.parsed_message_3.bytes);
  TEST_ASSERT_EQUAL(reduced_length, result.parsed_message_3.length);
}

void test_parser_fails_when_buffer_is_invalid(void) {
  const struct com_readonly_buffer invalid_buf = {.bytes = NULL, .length = 5};

  const struct srv_edhoc_parse_message_3_result result =
      srv_edhoc_parse_message_3(invalid_buf, &env.context);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_PARSE_ERR_INVALID_REQUEST_BUFFER,
                    result.status);
}

void test_parser_fails_when_context_is_null(void) {
  const struct srv_edhoc_parse_message_3_result result =
      srv_edhoc_parse_message_3(env.valid_request, NULL);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_PARSE_ERR_NULL_EDHOC_CONTEXT, result.status);
}

void test_parser_fails_when_connection_id_extraction_fails(void) {
  tst_srv_m3_parser_set_extraction_failure();

  const struct srv_edhoc_parse_message_3_result result =
      srv_edhoc_parse_message_3(env.valid_request, &env.context);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_PARSE_ERR_CON_ID_EXTRACTION_FAILED,
                    result.status);
}

void test_parser_fails_on_connection_id_mismatch(void) {
  tst_srv_m3_parser_set_cid_mismatch();

  const struct srv_edhoc_parse_message_3_result result =
      srv_edhoc_parse_message_3(env.valid_request, &env.context);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_PARSE_ERR_UNEXPECTED_CONNECTION_ID,
                    result.status);
}