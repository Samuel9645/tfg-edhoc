/**
 * @file tests.c
 *
 * @brief Unit tests for the CoAP server EDHOC dispatcher with dependency
 * injection.
 *
 * @see [RFC 9528
 * A.2](https://datatracker.ietf.org/doc/html/rfc9528#appendix-A.2.1) for
 * expected behavior of the dispatcher in handling EDHOC messages and error
 * conditions.
 *
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */
#include <coap3/coap.h>
#include <string.h>
#include <unity.h>

#include "coap/server/dispatch/create_base_dependencies.h"

static coap_session_t* dummy_session = NULL;
static coap_pdu_t* dummy_request = NULL;
static coap_pdu_t* dummy_response = NULL;
static int fake_session_memory = 0;

void setUp(void) {
  dummy_session = (coap_session_t*)&fake_session_memory;
  dummy_request = coap_pdu_init(COAP_MESSAGE_CON, COAP_EMPTY_CODE, 0, 0);
  dummy_response = coap_pdu_init(COAP_MESSAGE_CON, COAP_EMPTY_CODE, 0, 0);
}

void tearDown(void) {
  if (dummy_request) {
    coap_delete_pdu(dummy_request);
  }
  if (dummy_response) {
    coap_delete_pdu(dummy_response);
  }
  dummy_request = NULL;
  dummy_response = NULL;
  dummy_session = NULL;
}

void test_server_responds_with_bad_request_for_malformed_edhoc_message(void) {
  coap_server_edhoc_dispatch_deps_t deps = create_base_dependencies();
  deps.extract_payload_if_valid_edhoc_request = failed_extract_payload_stub;

  coap_server_dispatch_edhoc_post_with_dependencies(
      dummy_session, dummy_request, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_internal_error_on_server_side_failure(void) {
  coap_server_edhoc_dispatch_deps_t deps = create_base_dependencies();
  deps.add_edhoc_response_options = failed_add_response_options_stub;

  coap_server_dispatch_edhoc_post_with_dependencies(
      dummy_session, dummy_request, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_internal_error_if_context_already_exists(void) {
  coap_server_edhoc_dispatch_deps_t deps = create_base_dependencies();
  deps.get_session_app_data = get_non_null_session_app_data_stub;

  coap_server_dispatch_edhoc_post_with_dependencies(
      dummy_session, dummy_request, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}

void test_server_sends_changed_response_for_valid_message_1(void) {
  coap_server_edhoc_dispatch_deps_t deps = create_base_dependencies();
  deps.get_session_app_data = get_null_session_app_data_stub;

  coap_server_dispatch_edhoc_post_with_dependencies(
      dummy_session, dummy_request, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_CHANGED,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_bad_request_for_message_3_without_active_context(
    void) {
  coap_server_edhoc_dispatch_deps_t deps = create_base_dependencies();
  deps.is_message_1 = is_not_message_1_stub;
  deps.is_message_3 = is_message_3_stub;
  deps.get_session_app_data = get_null_session_app_data_stub;

  coap_server_dispatch_edhoc_post_with_dependencies(
      dummy_session, dummy_request, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_server_sends_changed_response_for_valid_message_3(void) {
  coap_server_edhoc_dispatch_deps_t deps = create_base_dependencies();
  deps.is_message_1 = is_not_message_1_stub;
  deps.is_message_3 = is_message_3_stub;
  deps.get_session_app_data = get_non_null_session_app_data_stub;

  coap_server_dispatch_edhoc_post_with_dependencies(
      dummy_session, dummy_request, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_CHANGED,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_internal_error_for_unrecognized_message_format(
    void) {
  coap_server_edhoc_dispatch_deps_t deps = create_base_dependencies();
  deps.is_message_1 = is_not_message_1_stub;
  deps.is_message_3 = is_not_message_3_stub;
  deps.get_session_app_data = get_non_null_session_app_data_stub;

  coap_server_dispatch_edhoc_post_with_dependencies(
      dummy_session, dummy_request, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}