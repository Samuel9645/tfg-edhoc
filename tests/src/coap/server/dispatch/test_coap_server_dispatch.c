/**
 * @file
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
#include <edhoc.h>
#include <unity.h>

#include "coap/server/dispatch/tst_create_base_dependencies.h"
#include "coap/server/dispatch/tst_srv_dispatch_stubs.h"

static coap_session_t* dummy_session = NULL;
static coap_pdu_t* dummy_request = NULL;
static coap_pdu_t* dummy_response = NULL;
static const struct edhoc_credentials DUMMY_TEST_CREDS = {0};
static int fake_session_memory = 0;

void setUp(void) {
  coap_startup();
  coap_set_log_level(COAP_LOG_CRIT);
  dummy_session = (coap_session_t*)&fake_session_memory;
  dummy_request = coap_pdu_init(COAP_MESSAGE_CON, COAP_EMPTY_CODE, 0, 0);
  dummy_response = coap_pdu_init(COAP_MESSAGE_CON, COAP_EMPTY_CODE, 0, 0);
}

void tearDown(void) {
  if (dummy_request != NULL) {
    coap_delete_pdu(dummy_request);
    dummy_request = NULL;
  }
  if (dummy_response != NULL) {
    coap_delete_pdu(dummy_response);
    dummy_response = NULL;
  }
  dummy_session = NULL;
  coap_cleanup();
}

void test_server_sends_changed_response_for_valid_message_1(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.get_session_app_data = stb_srv_coap_get_session_null;

  srv_coap_dispatch_post_with_dependencies(
      dummy_session, dummy_request, &DUMMY_TEST_CREDS, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_CHANGED,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_bad_request_for_malformed_edhoc_message(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.parse_edhoc_request = stb_srv_coap_parse_edhoc_request_fail;

  srv_coap_dispatch_post_with_dependencies(
      dummy_session, dummy_request, &DUMMY_TEST_CREDS, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_internal_error_on_server_side_failure(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.add_edhoc_response_options = stb_srv_coap_add_options_fail;

  srv_coap_dispatch_post_with_dependencies(
      dummy_session, dummy_request, &DUMMY_TEST_CREDS, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_bad_request_if_context_already_exists_for_message_1(
    void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.get_session_app_data = stb_srv_coap_get_session_valid;

  srv_coap_dispatch_post_with_dependencies(
      dummy_session, dummy_request, &DUMMY_TEST_CREDS, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_error_when_message_1_processing_fails(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.get_session_app_data = stb_srv_coap_get_session_null;
  deps.process_message_1_result = stb_srv_coap_process_m1_protocol_failure;

  srv_coap_dispatch_post_with_dependencies(
      dummy_session, dummy_request, &DUMMY_TEST_CREDS, dummy_response, &deps);

  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_internal_error_if_adding_response_payload_fails(
    void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.get_session_app_data = stb_srv_coap_get_session_null;
  deps.handle_message_1 = stb_srv_edhoc_m1_responder_ok;
  deps.add_response_payload = stb_srv_coap_add_payload_failure;

  srv_coap_dispatch_post_with_dependencies(
      dummy_session, dummy_request, &DUMMY_TEST_CREDS, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_bad_request_for_message_3_without_active_context(
    void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.parse_message_1 = stb_srv_edhoc_parse_m1_protocol_failure;
  deps.parse_message_3 = stb_srv_edhoc_parse_message_3_ok;
  deps.get_session_app_data = stb_srv_coap_get_session_null;

  srv_coap_dispatch_post_with_dependencies(
      dummy_session, dummy_request, &DUMMY_TEST_CREDS, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_server_sends_changed_response_for_valid_message_3(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.parse_message_1 = stb_srv_edhoc_parse_m1_protocol_failure;
  deps.parse_message_3 = stb_srv_edhoc_parse_message_3_ok;
  deps.get_session_app_data = stb_srv_coap_get_session_valid;

  srv_coap_dispatch_post_with_dependencies(
      dummy_session, dummy_request, &DUMMY_TEST_CREDS, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_CHANGED,
                    coap_pdu_get_code(dummy_response));
}

void test_server_responds_with_bad_request_for_unrecognized_message_format(
    void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.parse_message_1 = stb_srv_edhoc_parse_m1_protocol_failure;
  deps.parse_message_3 = stb_srv_edhoc_parse_m3_protocol_failure;
  deps.get_session_app_data = stb_srv_coap_get_session_valid;

  srv_coap_dispatch_post_with_dependencies(
      dummy_session, dummy_request, &DUMMY_TEST_CREDS, dummy_response, &deps);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}