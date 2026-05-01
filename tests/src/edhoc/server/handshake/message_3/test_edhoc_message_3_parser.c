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

#include "common/com_data_models.h"
#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"
#include "edhoc/server/handshake/message_3/srv_m3_parser.h"
#include "edhoc/server/handshake/mocks/message_3/tst_srv_mock_m3_parser_deps.h"

enum { TEST_MSG_3_PAYLOAD_LENGTH = 5, TST_M3_PARSER_ERROR_BUFFER_SIZE = 256 };

static const uint8_t DUMMY_REQUEST_BUFFER[TEST_MSG_3_PAYLOAD_LENGTH] = {0};

static struct {
  struct com_readonly_buffer valid_request;
  struct edhoc_context context;
  uint8_t error_message[TST_M3_PARSER_ERROR_BUFFER_SIZE];
  struct com_writable_buffer error_buffer_view;
} test_env = {
    .valid_request = {.bytes = DUMMY_REQUEST_BUFFER,
                      .length = TEST_MSG_3_PAYLOAD_LENGTH},
    .error_buffer_view = {.capacity = TST_M3_PARSER_ERROR_BUFFER_SIZE}};

void setUp(void) {
  tst_srv_edhoc_message_3_parser_reset_stub_results();
  memset(test_env.error_message, 0, TST_M3_PARSER_ERROR_BUFFER_SIZE);
  test_env.error_buffer_view.bytes = test_env.error_message;
}

void test_parser_advances_pointers_on_success(void) {
  const uint8_t* advanced_pointed = DUMMY_REQUEST_BUFFER + 2;
  const int reduced_length = TEST_MSG_3_PAYLOAD_LENGTH - 2;
  tst_srv_m3_parser_set_success_data(
      (struct edhoc_extracted_fields){.edhoc_message_ptr = advanced_pointed,
                                      .edhoc_message_size = reduced_length});

  const struct srv_edhoc_parse_message_3_result result =
      srv_edhoc_parse_message_3(test_env.valid_request, &test_env.context,
                                test_env.error_buffer_view);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_PARSE_OK, result.status);
  TEST_ASSERT_EQUAL_PTR(advanced_pointed, result.buffer.bytes);
  TEST_ASSERT_EQUAL(reduced_length, result.buffer.length);
}

void test_parser_fails_and_populates_error_when_buffer_is_invalid(void) {
  const struct com_readonly_buffer invalid_buf = {.bytes = NULL, .length = 5};

  const struct srv_edhoc_parse_message_3_result result =
      srv_edhoc_parse_message_3(invalid_buf, &test_env.context,
                                test_env.error_buffer_view);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_PARSE_ERR_EMPTY_REQUEST_BUFFER,
                    result.status);
  tst_edhoc_assert_encoded_error_matches(
      result.buffer, "Message 3 Parser error: Empty request buffer",
      EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_parser_fails_and_populates_error_when_context_is_null(void) {
  const struct srv_edhoc_parse_message_3_result result =
      srv_edhoc_parse_message_3(test_env.valid_request, NULL,
                                test_env.error_buffer_view);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_PARSE_ERR_NULL_EDHOC_CONTEXT, result.status);
  tst_edhoc_assert_encoded_error_matches(result.buffer,
                                         "Message 3 Parser error: Null context",
                                         EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_parser_fails_and_populates_error_when_connection_id_extraction_fails(
    void) {
  tst_srv_m3_parser_set_extraction_failure();

  const struct srv_edhoc_parse_message_3_result result =
      srv_edhoc_parse_message_3(test_env.valid_request, &test_env.context,
                                test_env.error_buffer_view);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_PARSE_ERR_CON_ID_EXTRACTION_FAILED,
                    result.status);
  tst_edhoc_assert_encoded_error_matches(
      result.buffer, "Message 3 Parser error: Connection ID extraction failed",
      EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_parser_fails_and_populates_error_on_connection_id_mismatch(void) {
  tst_srv_m3_parser_set_cid_mismatch();

  const struct srv_edhoc_parse_message_3_result result =
      srv_edhoc_parse_message_3(test_env.valid_request, &test_env.context,
                                test_env.error_buffer_view);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_PARSE_ERR_UNEXPECTED_CONNECTION_ID,
                    result.status);
  tst_edhoc_assert_encoded_error_matches(
      result.buffer, "Message 3 Parser error: Unexpected connection ID",
      EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_parser_fails_on_invalid_error_buffer(void) {
  const struct com_writable_buffer invalid_error_buffer = {.bytes = NULL,
                                                           .capacity = 0};

  const struct srv_edhoc_parse_message_3_result result =
      srv_edhoc_parse_message_3(test_env.valid_request, &test_env.context,
                                invalid_error_buffer);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_PARSE_ERR_INVALID_ERROR_BUFFER,
                    result.status);
  TEST_ASSERT_NULL(result.buffer.bytes);
  TEST_ASSERT_EQUAL(0, result.buffer.length);
}