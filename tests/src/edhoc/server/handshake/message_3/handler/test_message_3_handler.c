/**
 * @file
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

#include "edhoc/common/tst_mock_edhoc_error.h"
#include "edhoc/server/handshake/message_3/handler/tst_srv_m3_handler_env.h"
#include "edhoc/server/handshake/message_3/handler/tst_srv_mock_m3_handler_deps.h"

static struct tst_edh_srv_message_3_env env = {0};

void setUp(void) {
  tst_edh_srv_message_3_reset_stub_results();
  memset(&env, 0, sizeof(env));
  tst_edh_srv_message_3_setup_env(&env);
}

void test_handler_ok_on_valid_data(void) {
  const enum edh_srv_message_3_handler_status status =
      edh_srv_handle_message_3(env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG3_HDL_OK, status);
  tst_edh_srv_m3_assert_handler_writes_message_4_in_buffer(env.response);
}

void test_handler_fails_on_invalid_data(void) {
  const struct edh_srv_message_3_request no_context = {
      .parsed_message_3 = env.request.parsed_message_3};
  const struct edh_srv_message_3_request no_buffer = {.edhoc_context =
                                                          &env.context};
  struct edh_srv_message_3_request empty_request = env.request;
  empty_request.parsed_message_3.length = 0;
  struct com_writable_buffer unwritable_response = env.response;
  unwritable_response.capacity = 0;

  const struct {
    const char* description;
    struct edh_srv_message_3_request request;
    struct com_writable_buffer* response;
    enum edh_srv_message_3_handler_status expected;
    bool should_check_error_payload;
  } test_cases[] = {
      {"response is NULL", env.request, NULL,
       EDH_MSG3_HDL_ERR_INVALID_RESPONSE_BUFFER, false},
      {"response has 0 capacity", env.request, &unwritable_response,
       EDH_MSG3_HDL_ERR_INVALID_RESPONSE_BUFFER, false},
      {"context is NULL", no_context, &env.response,
       EDH_MSG3_HDL_ERR_NULL_EDHOC_CONTEXT, true},
      {"request buffer is NULL", no_buffer, &env.response,
       EDH_MSG3_HDL_ERR_INVALID_PARSED_MESSAGE_3, true},
      {"request payload is empty (len 0)", empty_request, &env.response,
       EDH_MSG3_HDL_ERR_INVALID_PARSED_MESSAGE_3, true}};

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    tst_edh_srv_message_3_reset_stub_results();
    const enum edh_srv_message_3_handler_status status =
        edh_srv_handle_message_3(test_cases[i].request, test_cases[i].response);

    TEST_ASSERT_EQUAL_MESSAGE(test_cases[i].expected, status,
                              test_cases[i].description);
    if (test_cases[i].should_check_error_payload) {
      tst_edh_srv_assert_handler_writes_error_in_buffer(env.response);
    }
  }
}

void test_handler_fails_when_message_3_processing_fails(void) {
  tst_edh_srv_m1_set_message_3_process_failure();

  const enum edh_srv_message_3_handler_status status =
      edh_srv_handle_message_3(env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG3_HDL_ERR_MESSAGE_3_PROCESS_FAILED, status);
  tst_edh_srv_assert_handler_writes_error_in_buffer(env.response);
}

void test_handler_fails_when_message_4_composition_fails(void) {
  tst_edh_srv_m1_set_message_4_compose_failure();

  const enum edh_srv_message_3_handler_status status =
      edh_srv_handle_message_3(env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_FAILED, status);
  tst_edh_srv_assert_handler_writes_error_in_buffer(env.response);
}

void test_handler_fails_when_message_4_composition_produces_empty_buffer(void) {
  tst_edh_srv_m3_set_message_4_compose_empty_length();

  const enum edh_srv_message_3_handler_status status =
      edh_srv_handle_message_3(env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_EMPTY, status);
  tst_edh_srv_assert_handler_writes_error_in_buffer(env.response);
}
