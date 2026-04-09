
/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Handler tests for EDHOC server message 1.
 * This file uses stubs to simulate different outcomes from the underlying EDHOC
 * library functions.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

#include <coap3/coap.h>
#include <edhoc.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "edhoc/common/tst_mock_add_edhoc_error_info.h"
#include "edhoc/config.h"
#include "edhoc/server/handshake/message_1/handler/tst_srv_m1_handler_env.h"
#include "edhoc/server/handshake/message_1/handler/tst_srv_mock_m1_handler_deps.h"

static struct tst_message_1_handler_env env = {0};

void setUp(void) {
  tst_edh_srv_m1_reset_stub_results();
  memset(&env, 0, sizeof(env));
  tst_edh_clear_message_1_setup_env(&env);
}

void test_handler_ok_for_valid_data(void) {
  const struct edh_srv_message_1_handler_result result =
      edh_srv_handle_message_1(env.valid_request, &env.response);

  TEST_ASSERT_EQUAL(EDH_SRV_MSG1_HDL_OK, result.status);
  tst_edh_srv_m1_assert_handler_writes_message_2_in_buffer(env.response);

  TEST_ASSERT_NOT_NULL(result.edhoc_ctx);
  free(result.edhoc_ctx);
}

static void ensure_context_is_freed_on_failure(
    const struct edh_srv_message_1_handler_result result) {
  TEST_ASSERT_NULL_MESSAGE(result.edhoc_ctx,
                           "Memory Leak: edhoc_ctx was not NULL on failure");
}

void test_handler_fails_on_invalid_data(void) {
  const struct edh_srv_message_1_request empty_request = {0};
  const struct edhoc_credentials dummy_credentials = {0};
  const struct edh_srv_message_1_request empty_request_with_credentials = {
      .credentials = &dummy_credentials};
  struct com_writable_buffer empty_response = {0};

  const struct {
    const char* description;
    struct edh_srv_message_1_request request;
    struct com_writable_buffer* response;
    enum edh_srv_message_1_handler_status expected_status;
  } test_cases[] = {
      {"empty request payload", empty_request_with_credentials, &env.response,
       EDH_SRV_MSG1_HDL_ERR_INVALID_REQUEST_BUFFER},
      {"response buffer is NULL", env.valid_request, NULL,
       EDH_SRV_MSG1_HDL_ERR_INVALID_RESPONSE_BUFFER},
      {"response buffer is empty/invalid", env.valid_request, &empty_response,
       EDH_SRV_MSG1_HDL_ERR_INVALID_RESPONSE_BUFFER},
      {"credentials are missing", empty_request, &env.response,
       EDH_SRV_MSG1_HDL_ERR_NULL_CREDENTIALS},
  };

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    tst_edh_srv_m1_reset_stub_results();
    const struct edh_srv_message_1_handler_result result =
        edh_srv_handle_message_1(test_cases[i].request, test_cases[i].response);

    TEST_ASSERT_EQUAL_MESSAGE(test_cases[i].expected_status, result.status,
                              test_cases[i].description);

    if (test_cases[i].response != NULL &&
        test_cases[i].response->bytes != NULL) {
      tst_edh_srv_assert_handler_writes_error_in_buffer(
          *test_cases[i].response);
    }
    ensure_context_is_freed_on_failure(result);
  }
}

void test_handler_fails_on_library_errors(void) {
  const struct {
    const char* description;
    void (*setup_scenario)(void);
    enum edh_srv_message_1_handler_status expected_status;
  } cases[] = {
      {"setup fails", tst_edh_srv_m1_set_setup_failure,
       EDH_SRV_MSG1_HDL_ERR_EDHOC_CONTEXT_SETUP_FAILED},
      {"processing fails", tst_edh_srv_m1_set_message_1_process_failure,
       EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED},
      {"composition fails", tst_edh_srv_m1_set_message_2_compose_failure,
       EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED},
  };

  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
    tst_edh_srv_m1_reset_stub_results();
    cases[i].setup_scenario();

    const struct edh_srv_message_1_handler_result result =
        edh_srv_handle_message_1(env.valid_request, &env.response);

    TEST_ASSERT_EQUAL_MESSAGE(cases[i].expected_status, result.status,
                              cases[i].description);
  }
}

void test_handler_fails_when_message_2_composition_produces_empty_buffer(void) {
  tst_edh_srv_m1_set_message_2_compose_empty_length();

  const struct edh_srv_message_1_handler_result result =
      edh_srv_handle_message_1(env.valid_request, &env.response);

  TEST_ASSERT_EQUAL(EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_EMPTY,
                    result.status);
  tst_edh_srv_assert_handler_writes_error_in_buffer(env.response);
  ensure_context_is_freed_on_failure(result);
}