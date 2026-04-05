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

#include "edhoc/server/handshake/message_3/parser/tst_srv_m3_parser_stubs.h"
#include "edhoc/server/handshake/message_3/srv_m3_parser.h"

enum { TEST_MSG_3_PAYLOAD_LENGTH = 5 };

static struct {
  uint8_t data[TEST_MSG_3_PAYLOAD_LENGTH];
  struct com_readonly_buffer request_buffer;
  struct edhoc_context context;
} env;

void setUp(void) {
  tst_edh_srv_message_3_parser_reset_stub_results();
  const uint8_t mock_data[TEST_MSG_3_PAYLOAD_LENGTH] = {0x01, 0x02, 0x03, 0x04,
                                                        0x05};
  memcpy(env.data, mock_data, TEST_MSG_3_PAYLOAD_LENGTH);
  env.request_buffer = (struct com_readonly_buffer){
      .bytes = env.data,
      .length = TEST_MSG_3_PAYLOAD_LENGTH,
  };
  memset(&env.context, 0, sizeof(struct edhoc_context));
}

void test_parser_advances_pointers_on_success(void) {
  const uint8_t* advanced_pointed = env.data + 2;
  const int reduced_length = TEST_MSG_3_PAYLOAD_LENGTH - 2;
  tst_edh_srv_message_3_extracted_fields_result =
      (struct edhoc_extracted_fields){.edhoc_message_ptr = advanced_pointed,
                                      .edhoc_message_size = reduced_length};

  const struct edh_srv_parse_message_3_result result =
      edh_srv_parse_message_3(env.request_buffer, &env.context);

  TEST_ASSERT_EQUAL(EDH_SRV_MSG3_PARSE_OK, result.status);
  TEST_ASSERT_EQUAL_PTR(advanced_pointed, result.parsed_message_3.bytes);
  TEST_ASSERT_EQUAL(reduced_length, result.parsed_message_3.length);
}

void test_parser_fails_when_buffer_is_invalid(void) {
  const struct com_readonly_buffer invalid_buf = {.bytes = NULL, .length = 5};

  const struct edh_srv_parse_message_3_result result =
      edh_srv_parse_message_3(invalid_buf, &env.context);

  TEST_ASSERT_EQUAL(EDH_SRV_MSG3_PARSE_ERR_INVALID_REQUEST_BUFFER,
                    result.status);
}

void test_parser_fails_when_context_is_null(void) {
  const struct edh_srv_parse_message_3_result result =
      edh_srv_parse_message_3(env.request_buffer, NULL);

  TEST_ASSERT_EQUAL(EDH_SRV_MSG3_PARSE_ERR_NULL_EDHOC_CONTEXT, result.status);
}

void test_parser_fails_when_connection_id_extraction_fails(void) {
  tst_edh_srv_message_3_parser_stub_extract_result =
      EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;

  const struct edh_srv_parse_message_3_result result =
      edh_srv_parse_message_3(env.request_buffer, &env.context);

  TEST_ASSERT_EQUAL(EDH_SRV_MSG3_PARSE_ERR_CON_ID_EXTRACTION_FAILED,
                    result.status);
}

void test_parser_fails_on_connection_id_mismatch(void) {
  tst_edh_srv_message_3_parser_stub_equal_result = false;

  const struct edh_srv_parse_message_3_result result =
      edh_srv_parse_message_3(env.request_buffer, &env.context);

  TEST_ASSERT_EQUAL(EDH_SRV_MSG3_PARSE_ERR_UNEXPECTED_CONNECTION_ID,
                    result.status);
}