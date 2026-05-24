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
#include <coap3/coap_net_internal.h>
#include <edhoc.h>
#include <unity.h>

#include "coap/server/dispatch/mocks/srv_mock_dispatch_deps.h"
#include "coap/server/dispatch/tst_srv_dispatch_stubs.h"
#include "edhoc/common/tst_edhoc_params.h"

static coap_pdu_t* dummy_request = NULL;
static coap_pdu_t* dummy_response = NULL;
static coap_context_t dummy_context = {0};

void setUp(void) {
  coap_startup();
  coap_set_log_level(COAP_LOG_CRIT);
  tst_srv_dispatch_reset_mock();
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
  coap_cleanup();
}

void test_changed_response_for_valid_message_1(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.is_message_1 = stb_srv_coap_is_message_1_true;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_CHANGED,
                    coap_pdu_get_code(dummy_response));
  srv_dispatch_cleanup_last_context();
}

void test_bad_request_for_malformed_edhoc_message(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.parse_edhoc_request = stb_srv_coap_parse_edhoc_request_fail;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_internal_error_on_server_side_failure(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.add_edhoc_response_options = stb_srv_coap_add_options_fail;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}

void test_internal_error_when_message_1_context_allocation_fails(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.is_message_1 = stb_srv_coap_is_message_1_true;
  tst_srv_set_calloc_failure();

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);

  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}

void test_internal_error_when_message_1_context_setup_fails(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.is_message_1 = stb_srv_coap_is_message_1_true;
  tst_srv_set_setup_context_failure();

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);

  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}

void test_bad_request_on_parse_m1_failure(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.extract_message_1 = stb_srv_coap_extract_message_1_format_failure;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_internal_error_on_extract_cid_failure(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.is_message_1 = stb_srv_coap_is_message_1_false;
  deps.extract_cid = stb_srv_coap_extract_cid_failure;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}

void test_internal_error_if_context_already_exists_for_message_1(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.is_message_1 = stb_srv_coap_is_message_1_true;
  deps.set_context_by_cid = stb_srv_session_set_context_duplicate_cid;
  deps.respond_to_message_3 = stb_srv_edhoc_m3_responder_protocol_failure;
  deps.process_message_3_result = stb_srv_coap_process_m3_protocol_failure;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}

void test_error_when_message_1_processing_fails(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.is_message_1 = stb_srv_coap_is_message_1_true;
  deps.process_message_1_result = stb_srv_coap_process_m1_protocol_failure;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);

  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_internal_error_if_adding_response_payload_fails(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.is_message_1 = stb_srv_coap_is_message_1_false;
  deps.respond_to_message_1 = stb_srv_edhoc_m1_responder_ok;
  deps.add_response_payload = stb_srv_coap_add_payload_failure;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}

void test_bad_request_for_message_3_without_active_context(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.is_message_1 = stb_srv_coap_is_message_1_false;
  deps.respond_to_message_1 = stb_srv_edhoc_m1_responder_ok;
  deps.process_message_1_result = stb_srv_coap_process_m1_ok;
  deps.get_context_by_cid = stb_srv_session_get_context_not_found;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_changed_response_for_valid_message_3(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.is_message_1 = stb_srv_coap_is_message_1_true;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_CHANGED,
                    coap_pdu_get_code(dummy_response));
}

void test_bad_request_for_unrecognized_message_format(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();
  deps.is_message_1 = stb_srv_coap_is_message_1_false;
  deps.respond_to_message_3 = stb_srv_edhoc_m3_responder_protocol_failure;
  deps.process_message_3_result = stb_srv_coap_process_m3_protocol_failure;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_internal_error_when_session_storage_is_full(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();

  deps.is_message_1 = stb_srv_coap_is_message_1_true;
  deps.set_context_by_cid = stb_srv_session_set_context_duplicate_cid;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);

  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}

void test_bad_request_when_session_not_found_for_message_3(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();

  deps.is_message_1 = stb_srv_coap_is_message_1_false;
  deps.get_context_by_cid = stb_srv_session_get_context_not_found;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);

  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_bad_request_when_connection_id_mismatch_occurs(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();

  deps.is_message_1 = stb_srv_coap_is_message_1_false;
  deps.get_context_by_cid = stb_srv_session_get_context_ok;
  deps.connection_id_is_expected = stb_srv_coap_connection_id_is_expected_false;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);

  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_BAD_REQUEST,
                    coap_pdu_get_code(dummy_response));
}

void test_internal_error_when_oscore_binding_fails(void) {
  struct srv_coap_dispatch_deps deps =
      test_srv_coap_dispatch_create_base_dependencies();

  deps.is_message_1 = stb_srv_coap_is_message_1_false;
  deps.bind_oscore_session = stb_srv_oscore_bind_session_failure;

  srv_coap_dispatch_post_with_dependencies(
      dummy_request, tst_edhoc_get_method_0_suite_0_params(), dummy_response,
      &deps, &dummy_context);

  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR,
                    coap_pdu_get_code(dummy_response));
}