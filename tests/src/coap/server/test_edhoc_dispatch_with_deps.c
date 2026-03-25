/**
 * @file test_edhoc_dispatch_with_deps.c
 *
 * @brief Unit tests for the CoAP server EDHOC dispatcher with dependency
 * injection.
 *
 * @see [RFC 9528
 * A.2](https://datatracker.ietf.org/doc/html/rfc9528#appendix-A.2.1) for
 * expected behavior of the dispatcher in handling EDHOC messages and error
 * conditions.
 */
#include <coap3/coap.h>
#include <string.h>
#include <unity.h>

#include "coap/server/test_edhoc_dispatch_deps_builder.h"

/**
 * Since libcoap's coap_pdu_t and coap_session_t are opaque structs, we can't
 * directly instantiate them in our tests. Instead, we create raw byte arrays to
 * serve as backing memory for these structs, and then cast pointers to these
 * arrays as needed.
 */
enum { INITIALIZATION_SIZE = 512 };
static uint8_t session_mem[INITIALIZATION_SIZE];
static uint8_t request_mem[INITIALIZATION_SIZE];
static uint8_t response_mem[INITIALIZATION_SIZE];

static coap_session_t* dummy_session = (coap_session_t*)session_mem;
static coap_pdu_t* dummy_request = (coap_pdu_t*)request_mem;
static coap_pdu_t* dummy_response = (coap_pdu_t*)response_mem;

void setUp(void) {
  memset(session_mem, 0, sizeof(session_mem));
  memset(request_mem, 0, sizeof(request_mem));
  memset(response_mem, 0, sizeof(response_mem));
}

void tearDown(void) {}

void test_server_responds_with_bad_request_for_malformed_edhoc_message(void) {
  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.extract_payload_if_valid_edhoc_request = failed_extract_payload_stub;

  coap_server_dispatch_edhoc_post_with_deps(dummy_session, dummy_request,
                                            dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_internal_error_on_server_side_failure(void) {
  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.add_edhoc_response_options = failed_add_response_options_stub;

  coap_server_dispatch_edhoc_post_with_deps(dummy_session, dummy_request,
                                            dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_internal_error_if_context_already_exists(void) {
  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.get_session_app_data = get_non_null_session_app_data_stub;

  coap_server_dispatch_edhoc_post_with_deps(dummy_session, dummy_request,
                                            dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}

void test_server_sends_changed_response_for_valid_message_1(void) {
  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.get_session_app_data = get_null_session_app_data_stub;

  coap_server_dispatch_edhoc_post_with_deps(dummy_session, dummy_request,
                                            dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_CHANGED,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_bad_request_for_message_3_without_active_context(
    void) {
  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.is_message_1 = is_not_message_1_stub;
  deps.is_message_3 = is_message_3_stub;
  deps.get_session_app_data = get_null_session_app_data_stub;

  coap_server_dispatch_edhoc_post_with_deps(dummy_session, dummy_request,
                                            dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_server_sends_changed_response_for_valid_message_3(void) {
  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.is_message_1 = is_not_message_1_stub;
  deps.is_message_3 = is_message_3_stub;
  deps.get_session_app_data = get_non_null_session_app_data_stub;

  coap_server_dispatch_edhoc_post_with_deps(dummy_session, dummy_request,
                                            dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_CHANGED,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_internal_error_for_unrecognized_message_format(
    void) {
  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.is_message_1 = is_not_message_1_stub;
  deps.is_message_3 = is_not_message_3_stub;
  deps.get_session_app_data = get_non_null_session_app_data_stub;

  coap_server_dispatch_edhoc_post_with_deps(dummy_session, dummy_request,
                                            dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_server_responds_with_bad_request_for_malformed_edhoc_message);
  RUN_TEST(test_server_responds_with_internal_error_on_server_side_failure);
  RUN_TEST(test_server_responds_with_internal_error_if_context_already_exists);
  RUN_TEST(test_server_sends_changed_response_for_valid_message_1);
  RUN_TEST(
      test_server_responds_with_bad_request_for_message_3_without_active_context);
  RUN_TEST(test_server_sends_changed_response_for_valid_message_3);
  RUN_TEST(
      test_server_responds_with_internal_error_for_unrecognized_message_format);
  return UNITY_END();
}
