/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Tests for CID extraction and validation from EDHOC Message 3.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

#include <edhoc.h>
#include <string.h>
#include <unity.h>

#include "coap/server/extract_edhoc_message/srv_coap_extract_m3.h"
#include "edhoc/server/handshake/mocks/message_3/tst_srv_mock_m3_extract_deps.h"

enum { TEST_MSG_3_PAYLOAD_LENGTH = 5 };

static const uint8_t REQUEST_BUFFER[TEST_MSG_3_PAYLOAD_LENGTH] = {0};

static struct {
  struct com_readonly_buffer valid_request;
  struct edhoc_context context;
} test_env = {.valid_request = {.bytes = REQUEST_BUFFER,
                                .length = TEST_MSG_3_PAYLOAD_LENGTH}};

void setUp(void) { tst_srv_edhoc_message_3_extract_reset_stub_results(); }

void test_extract_connection_id_advances_pointers_on_success(void) {
  const uint8_t* advanced_pointed = REQUEST_BUFFER + 2;
  const int reduced_length = TEST_MSG_3_PAYLOAD_LENGTH - 2;
  tst_srv_m3_extract_set_success_data(
      (struct edhoc_extracted_fields){.edhoc_message_ptr = advanced_pointed,
                                      .edhoc_message_size = reduced_length});

  const struct srv_coap_extract_connection_id_result result =
      srv_coap_extract_connection_id(test_env.valid_request);

  TEST_ASSERT_EQUAL(SRV_COAP_EXTRACT_CID_OK, result.status);
  TEST_ASSERT_EQUAL_PTR(advanced_pointed, result.message_payload.bytes);
  TEST_ASSERT_EQUAL(reduced_length, result.message_payload.length);
}

void test_extract_connection_id_fails_on_empty_buffer(void) {
  const struct com_readonly_buffer invalid_buf = {.bytes = NULL, .length = 5};

  const struct srv_coap_extract_connection_id_result result =
      srv_coap_extract_connection_id(invalid_buf);

  TEST_ASSERT_EQUAL(SRV_COAP_EXTRACT_CID_ERR_EMPTY_BUFFER, result.status);
}

void test_extract_connection_id_fails_on_extraction_error(void) {
  tst_srv_m3_extract_set_extraction_failure();

  const struct srv_coap_extract_connection_id_result result =
      srv_coap_extract_connection_id(test_env.valid_request);

  TEST_ASSERT_EQUAL(SRV_COAP_EXTRACT_CID_ERR_EXTRACT, result.status);
}

void test_connection_id_is_expected_returns_true_on_match(void) {
  const struct edhoc_connection_id extracted_cid = {
      .encode_type = EDHOC_CID_TYPE_ONE_BYTE_INTEGER, .int_value = 5};
  test_env.context.private_cid = extracted_cid;

  const bool result =
      srv_coap_connection_id_is_expected(&extracted_cid, &test_env.context);

  TEST_ASSERT_TRUE(result);
}

void test_connection_id_is_expected_returns_false_on_mismatch(void) {
  tst_srv_m3_extract_set_cid_mismatch();

  const struct edhoc_connection_id extracted_cid = {
      .encode_type = EDHOC_CID_TYPE_ONE_BYTE_INTEGER, .int_value = 5};

  const bool result =
      srv_coap_connection_id_is_expected(&extracted_cid, &test_env.context);

  TEST_ASSERT_FALSE(result);
}

void test_connection_id_is_expected_returns_false_on_null_context(void) {
  const struct edhoc_connection_id extracted_cid = {
      .encode_type = EDHOC_CID_TYPE_ONE_BYTE_INTEGER, .int_value = 5};

  const bool result = srv_coap_connection_id_is_expected(&extracted_cid, NULL);

  TEST_ASSERT_FALSE(result);
}
