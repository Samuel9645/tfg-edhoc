/**
 * @file test_edhoc_dispatch_with_deps.c
 *
 * @brief Unit tests for the CoAP server EDHOC dispatcher with dependency
 * injection.
 */
#include <unity.h>

#include "coap/server/test_edhoc_dispatch_deps_builder.h"

void setUp(void) {
  global_dispatch_trace = (dispatch_trace_t){0};
  global_validate_result = COAP_STATUS_SUCCESS;
  global_extracted_payload = global_message_1_payload;
  global_extracted_payload_len = sizeof(global_message_1_payload);
  global_expected_c_r = TEST_MESSAGE_3_C_R_BYTE;
}

void tearDown(void) {}

static void test_replies_changed_for_valid_message_1_in_forward_flow(void) {
  fake_session_t fake_session = {.app_data = NULL};
  fake_response_t fake_response = {.last_code = COAP_EMPTY_CODE,
                                   .set_response_code_call_count = 0};
  const coap_pdu_t* fake_request = (const coap_pdu_t*)&fake_session;

  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.handle_message_1 = handle_message_1_changed_with_payload;

  coap_server_dispatch_edhoc_post_with_deps((coap_session_t*)&fake_session,
                                            fake_request,
                                            (coap_pdu_t*)&fake_response, &deps);

  TEST_ASSERT_TRUE(global_dispatch_trace.validate_request_called);
  TEST_ASSERT_EQUAL(APPLICATION_CID_EDHOC_CBOR_SEQ,
                    global_dispatch_trace.expected_request_format);
  TEST_ASSERT_TRUE(global_dispatch_trace.add_response_options_called);
  TEST_ASSERT_EQUAL(APPLICATION_EDHOC_CBOR_SEQ,
                    global_dispatch_trace.response_options_format);
  TEST_ASSERT_TRUE(global_dispatch_trace.check_message_1_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.handle_message_1_request_called);
  TEST_ASSERT_FALSE(global_dispatch_trace.handle_message_3_request_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.add_response_payload_called);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_CHANGED, fake_response.last_code);
}

static void test_replies_changed_for_valid_message_3_with_prepended_c_r(void) {
  struct edhoc_context fake_ctx = {0};
  fake_session_t fake_session = {.app_data = &fake_ctx};
  fake_response_t fake_response = {.last_code = COAP_EMPTY_CODE,
                                   .set_response_code_call_count = 0};
  const coap_pdu_t* fake_request = (const coap_pdu_t*)&fake_session;

  global_extracted_payload = global_message_3_payload;
  global_extracted_payload_len = sizeof(global_message_3_payload);

  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.handle_message_3 = handle_message_3_changed_with_prepended_c_r;

  coap_server_dispatch_edhoc_post_with_deps((coap_session_t*)&fake_session,
                                            fake_request,
                                            (coap_pdu_t*)&fake_response, &deps);

  TEST_ASSERT_TRUE(global_dispatch_trace.validate_request_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.add_response_options_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.check_message_1_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.check_message_3_called);
  TEST_ASSERT_FALSE(global_dispatch_trace.handle_message_1_request_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.handle_message_3_request_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.add_response_payload_called);
  TEST_ASSERT_EQUAL_UINT32(
      TEST_DOUBLE_PAYLOAD_LEN,
      (uint32_t)global_dispatch_trace.response_payload_len);
  TEST_ASSERT_EQUAL_UINT8(TEST_MESSAGE_3_C_R_BYTE,
                          global_dispatch_trace.response_payload_first_byte);
  TEST_ASSERT_EQUAL_UINT8(TEST_MESSAGE_3_SUFFIX_BYTE,
                          global_dispatch_trace.response_payload_second_byte);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_CHANGED, fake_response.last_code);
}

static void test_replies_bad_request_when_edhoc_request_validation_fails(void) {
  fake_session_t fake_session = {.app_data = NULL};
  fake_response_t fake_response = {.last_code = COAP_EMPTY_CODE,
                                   .set_response_code_call_count = 0};
  const coap_pdu_t* fake_request = (const coap_pdu_t*)&fake_session;

  global_validate_result = COAP_STATUS_ERROR;

  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();

  coap_server_dispatch_edhoc_post_with_deps((coap_session_t*)&fake_session,
                                            fake_request,
                                            (coap_pdu_t*)&fake_response, &deps);

  TEST_ASSERT_TRUE(global_dispatch_trace.validate_request_called);
  TEST_ASSERT_FALSE(global_dispatch_trace.add_response_options_called);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST, fake_response.last_code);
}

static void test_replies_bad_request_for_message_3_without_context(void) {
  fake_session_t fake_session = {.app_data = NULL};
  fake_response_t fake_response = {.last_code = COAP_EMPTY_CODE,
                                   .set_response_code_call_count = 0};
  const coap_pdu_t* fake_request = (const coap_pdu_t*)&fake_session;

  global_extracted_payload = global_message_3_payload;
  global_extracted_payload_len = sizeof(global_message_3_payload);

  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();

  coap_server_dispatch_edhoc_post_with_deps((coap_session_t*)&fake_session,
                                            fake_request,
                                            (coap_pdu_t*)&fake_response, &deps);

  TEST_ASSERT_TRUE(global_dispatch_trace.add_response_options_called);
  TEST_ASSERT_EQUAL(APPLICATION_EDHOC_CBOR_SEQ,
                    global_dispatch_trace.response_options_format);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST, fake_response.last_code);
}

static void test_replies_internal_error_when_response_options_cannot_be_added(
    void) {
  fake_session_t fake_session = {.app_data = NULL};
  fake_response_t fake_response = {.last_code = COAP_EMPTY_CODE,
                                   .set_response_code_call_count = 0};
  const coap_pdu_t* fake_request = (const coap_pdu_t*)&fake_session;

  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.add_edhoc_response_options = add_edhoc_response_options_fail;

  coap_server_dispatch_edhoc_post_with_deps((coap_session_t*)&fake_session,
                                            fake_request,
                                            (coap_pdu_t*)&fake_response, &deps);

  TEST_ASSERT_TRUE(global_dispatch_trace.validate_request_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.add_response_options_called);
  TEST_ASSERT_EQUAL(APPLICATION_EDHOC_CBOR_SEQ,
                    global_dispatch_trace.response_options_format);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR, fake_response.last_code);
  TEST_ASSERT_EQUAL_UINT32(
      TEST_SINGLE_SET_CODE_CALL,
      (uint32_t)fake_response.set_response_code_call_count);
}

static void
test_replies_internal_error_when_message_1_fails_without_error_payload(void) {
  fake_session_t fake_session = {.app_data = NULL};
  fake_response_t fake_response = {.last_code = COAP_EMPTY_CODE,
                                   .set_response_code_call_count = 0};
  const coap_pdu_t* fake_request = (const coap_pdu_t*)&fake_session;

  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.handle_message_1 = handle_message_1_internal_error_with_empty_payload;

  coap_server_dispatch_edhoc_post_with_deps((coap_session_t*)&fake_session,
                                            fake_request,
                                            (coap_pdu_t*)&fake_response, &deps);

  TEST_ASSERT_TRUE(global_dispatch_trace.handle_message_1_request_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.add_response_options_called);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR, fake_response.last_code);
  TEST_ASSERT_EQUAL_UINT32(
      TEST_SINGLE_SET_CODE_CALL,
      (uint32_t)fake_response.set_response_code_call_count);
}

static void
test_replies_internal_error_with_error_payload_for_message_1_failure(void) {
  fake_session_t fake_session = {.app_data = NULL};
  fake_response_t fake_response = {.last_code = COAP_EMPTY_CODE,
                                   .set_response_code_call_count = 0};
  const coap_pdu_t* fake_request = (const coap_pdu_t*)&fake_session;

  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.handle_message_1 = handle_message_1_internal_error_with_payload;

  coap_server_dispatch_edhoc_post_with_deps((coap_session_t*)&fake_session,
                                            fake_request,
                                            (coap_pdu_t*)&fake_response, &deps);

  TEST_ASSERT_TRUE(global_dispatch_trace.handle_message_1_request_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.add_response_payload_called);
  TEST_ASSERT_EQUAL_UINT32(
      TEST_SINGLE_PAYLOAD_LEN,
      (uint32_t)global_dispatch_trace.response_payload_len);
  TEST_ASSERT_EQUAL_UINT8(TEST_ERROR_PAYLOAD_BYTE,
                          global_dispatch_trace.response_payload_first_byte);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR, fake_response.last_code);
}

static void test_replies_bad_request_with_prepended_c_r_for_message_3_failure(
    void) {
  struct edhoc_context fake_ctx = {0};
  fake_session_t fake_session = {.app_data = &fake_ctx};
  fake_response_t fake_response = {.last_code = COAP_EMPTY_CODE,
                                   .set_response_code_call_count = 0};
  const coap_pdu_t* fake_request = (const coap_pdu_t*)&fake_session;

  global_extracted_payload = global_message_3_payload;
  global_extracted_payload_len = sizeof(global_message_3_payload);

  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.handle_message_3 = handle_message_3_bad_request_with_error_payload;

  coap_server_dispatch_edhoc_post_with_deps((coap_session_t*)&fake_session,
                                            fake_request,
                                            (coap_pdu_t*)&fake_response, &deps);

  TEST_ASSERT_TRUE(global_dispatch_trace.check_message_3_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.handle_message_3_request_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.add_response_payload_called);
  TEST_ASSERT_EQUAL_UINT32(
      TEST_DOUBLE_PAYLOAD_LEN,
      (uint32_t)global_dispatch_trace.response_payload_len);
  TEST_ASSERT_EQUAL_UINT8(TEST_MESSAGE_3_C_R_BYTE,
                          global_dispatch_trace.response_payload_first_byte);
  TEST_ASSERT_EQUAL_UINT8(TEST_ERROR_PAYLOAD_BYTE,
                          global_dispatch_trace.response_payload_second_byte);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST, fake_response.last_code);
}

static void test_replies_internal_error_when_response_payload_cannot_be_added(
    void) {
  fake_session_t fake_session = {.app_data = NULL};
  fake_response_t fake_response = {.last_code = COAP_EMPTY_CODE,
                                   .set_response_code_call_count = 0};
  const coap_pdu_t* fake_request = (const coap_pdu_t*)&fake_session;

  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.handle_message_1 = handle_message_1_changed_with_payload;
  deps.add_response_payload = add_response_payload_fail;

  coap_server_dispatch_edhoc_post_with_deps((coap_session_t*)&fake_session,
                                            fake_request,
                                            (coap_pdu_t*)&fake_response, &deps);

  TEST_ASSERT_TRUE(global_dispatch_trace.add_response_payload_called);
  TEST_ASSERT_EQUAL_UINT32(
      TEST_SINGLE_PAYLOAD_LEN,
      (uint32_t)global_dispatch_trace.response_payload_len);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR, fake_response.last_code);
  TEST_ASSERT_EQUAL_UINT32(
      TEST_SINGLE_SET_CODE_CALL,
      (uint32_t)fake_response.set_response_code_call_count);
}

static void
test_replies_internal_error_when_message_1_arrives_with_existing_context(void) {
  struct edhoc_context fake_ctx = {0};
  fake_session_t fake_session = {.app_data = &fake_ctx};
  fake_response_t fake_response = {.last_code = COAP_EMPTY_CODE,
                                   .set_response_code_call_count = 0};
  const coap_pdu_t* fake_request = (const coap_pdu_t*)&fake_session;

  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();

  coap_server_dispatch_edhoc_post_with_deps((coap_session_t*)&fake_session,
                                            fake_request,
                                            (coap_pdu_t*)&fake_response, &deps);

  TEST_ASSERT_TRUE(global_dispatch_trace.validate_request_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.add_response_options_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.check_message_1_called);
  TEST_ASSERT_FALSE(global_dispatch_trace.handle_message_1_request_called);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR, fake_response.last_code);
}

static void test_replies_internal_error_for_invalid_or_unexpected_edhoc_message(
    void) {
  fake_session_t fake_session = {.app_data = NULL};
  fake_response_t fake_response = {.last_code = COAP_EMPTY_CODE,
                                   .set_response_code_call_count = 0};
  const coap_pdu_t* fake_request = (const coap_pdu_t*)&fake_session;

  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.is_message_1 = is_not_message_1_format;
  deps.is_message_3 = is_not_message_3_format;

  coap_server_dispatch_edhoc_post_with_deps((coap_session_t*)&fake_session,
                                            fake_request,
                                            (coap_pdu_t*)&fake_response, &deps);

  TEST_ASSERT_TRUE(global_dispatch_trace.validate_request_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.add_response_options_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.check_message_1_called);
  TEST_ASSERT_TRUE(global_dispatch_trace.check_message_3_called);
  TEST_ASSERT_FALSE(global_dispatch_trace.handle_message_1_request_called);
  TEST_ASSERT_FALSE(global_dispatch_trace.handle_message_3_request_called);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR, fake_response.last_code);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_replies_changed_for_valid_message_1_in_forward_flow);
  RUN_TEST(test_replies_changed_for_valid_message_3_with_prepended_c_r);
  RUN_TEST(test_replies_bad_request_when_edhoc_request_validation_fails);
  RUN_TEST(test_replies_bad_request_for_message_3_without_context);
  RUN_TEST(test_replies_internal_error_when_response_options_cannot_be_added);
  RUN_TEST(
      test_replies_internal_error_when_message_1_fails_without_error_payload);
  RUN_TEST(
      test_replies_internal_error_with_error_payload_for_message_1_failure);
  RUN_TEST(test_replies_bad_request_with_prepended_c_r_for_message_3_failure);
  RUN_TEST(test_replies_internal_error_when_response_payload_cannot_be_added);
  RUN_TEST(
      test_replies_internal_error_when_message_1_arrives_with_existing_context);
  RUN_TEST(test_replies_internal_error_for_invalid_or_unexpected_edhoc_message);
  return UNITY_END();
}
