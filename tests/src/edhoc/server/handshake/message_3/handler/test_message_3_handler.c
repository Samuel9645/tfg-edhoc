/**
 * @file test_message_3_handler.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Handler tests for EDHOC server message 3.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

#include <string.h>
#include <unity.h>

#include "edhoc/server/handshake/common/helpers.h"
#include "edhoc/server/handshake/message_3/handler/helpers.h"
#include "edhoc/server/handshake/message_3/handler/scenarios.h"
#include "edhoc/server/handshake/message_3/handler/stubs.h"

static tst_edh_srv_message_3_env_t env = {0};

void setUp(void) {
  tst_edh_srv_message_3_reset_stub_results();
  memset(&env, 0, sizeof(env));
  tst_edh_srv_message_3_setup_env(&env);
}

static void assert_handler_writes_error_payload(
    const edh_srv_message_3_handler_status_t status,
    const edh_srv_message_3_handler_status_t expected_status) {
  TEST_ASSERT_EQUAL(expected_status, status);
  TEST_ASSERT_EQUAL(TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_LEN, env.response.length);
  TEST_ASSERT_EQUAL_MEMORY(TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_PAYLOAD,
                           env.response.buffer,
                           TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_LEN);
}

void test_handler_returns_success_for_valid_message_3_data(void) {
  const edh_srv_message_3_handler_status_t status =
      edh_srv_handle_message_3(&env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG3_HDL_OK, status);
}

void test_handler_fails_on_invalid_data(void) {
  edh_srv_message_3_request_t empty_request = tst_empty_message_3_request();
  com_response_buffer_t empty_response = tst_edh_invalid_response();
  edh_srv_message_3_request_t request_without_extracted_fields =
      tst_message_3_request_without_extracted_fields(&env);
  edh_srv_message_3_request_t request_without_context =
      tst_message_3_request_without_context(&env);

  const message_3_handler_test_case_t test_cases[] = {
      {"both arguments are missing", NULL, NULL},
      {"request is missing", NULL, &env.response},
      {"response is missing", &env.request, NULL},
      {"request data is empty", &empty_request, &env.response},
      {"response buffer is empty", &env.request, &empty_response},
      {"extracted fields are missing", &request_without_extracted_fields,
       &env.response},
      {"connection context is missing", &request_without_context,
       &env.response},
  };

  const size_t num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < num_cases; i++) {
    const edh_srv_message_3_handler_status_t status =
        edh_srv_handle_message_3(test_cases[i].request, test_cases[i].response);

    TEST_ASSERT_EQUAL_MESSAGE(EDH_MSG3_HDL_ERR_INVALID_ARGS, status,
                              test_cases[i].description);
    if (test_cases[i].response) {
      tst_edh_srv_assert_response_clean(test_cases[i].response);
    }
  }
}

void test_handler_fails_when_message_3_processing_fails(void) {
  tst_edh_srv_message_3_stub_edhoc_process_result = EDHOC_ERROR_CRYPTO_FAILURE;

  const edh_srv_message_3_handler_status_t status =
      edh_srv_handle_message_3(&env.request, &env.response);

  assert_handler_writes_error_payload(
      status, EDH_MSG3_HDL_ERR_MESSAGE_3_PROCESS_FAILED);
}

void test_handler_fails_when_message_4_composition_fails(void) {
  tst_edh_srv_message_3_stub_edhoc_compose_result =
      EDHOC_ERROR_BUFFER_TOO_SMALL;

  const edh_srv_message_3_handler_status_t status =
      edh_srv_handle_message_3(&env.request, &env.response);

  assert_handler_writes_error_payload(
      status, EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_FAILED);
}
