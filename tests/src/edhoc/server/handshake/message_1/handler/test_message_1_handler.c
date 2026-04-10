
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

#include "edhoc/common/tst_mock_edhoc_error.h"
#include "edhoc/edhoc_config.h"
#include "edhoc/server/handshake/message_1/handler/tst_srv_mock_m1_handler_deps.h"
#include "edhoc/server/handshake/message_1/srv_m1_handler.h"

enum { TST_SRV_EDHOC_HND_BUF_LEN = 256 };

static const struct edhoc_credentials DUMMY_TEST_CREDS = {0};

static struct tst_message_1_handler_env {
  const uint8_t request_buffer[TST_SRV_EDHOC_HND_BUF_LEN];
  uint8_t response_buffer[TST_SRV_EDHOC_HND_BUF_LEN];
  const struct srv_edhoc_message_1_request valid_request;
  struct com_writable_buffer response;
} env = {.response = {.bytes = env.response_buffer,
                      .capacity = sizeof(env.response_buffer)},
         .valid_request = {.payload =
                               {
                                   .bytes = env.request_buffer,
                                   .length = sizeof(env.request_buffer),
                               },
                           .credentials = &DUMMY_TEST_CREDS}};

void setUp(void) {
  tst_srv_edhoc_m1_reset_stub_results();
  memset(env.response_buffer, 0, sizeof(env.response_buffer));
  env.response.bytes = env.response_buffer;
  env.response.length = 0;
}

void test_handler_ok_for_valid_data(void) {
  const struct srv_edhoc_message_1_handler_result result =
      srv_edhoc_handle_message_1(env.valid_request, &env.response);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG1_HDL_OK, result.status);
  tst_srv_edhoc_m1_assert_handler_writes_message_2_in_buffer(env.response);

  TEST_ASSERT_NOT_NULL(result.edhoc_ctx);
  free(result.edhoc_ctx);
}

static void ensure_context_is_freed_on_failure(
    const struct srv_edhoc_message_1_handler_result result) {
  TEST_ASSERT_NULL_MESSAGE(result.edhoc_ctx,
                           "Memory Leak: edhoc_ctx was not NULL on failure");
}

void test_handler_fails_on_invalid_data(void) {
  const struct srv_edhoc_message_1_request empty_request = {0};
  const struct edhoc_credentials dummy_credentials = {0};
  const struct srv_edhoc_message_1_request empty_request_with_credentials = {
      .credentials = &dummy_credentials};
  struct com_writable_buffer empty_response = {0};

  const struct {
    const char* description;
    struct srv_edhoc_message_1_request request;
    struct com_writable_buffer* response;
    enum srv_edhoc_message_1_handler_status expected_status;
  } test_cases[] = {
      {"empty request payload", empty_request_with_credentials, &env.response,
       SRV_EDHOC_MSG1_HDL_ERR_INVALID_REQUEST_BUFFER},
      {"response buffer is NULL", env.valid_request, NULL,
       SRV_EDHOC_MSG1_HDL_ERR_INVALID_RESPONSE_BUFFER},
      {"response buffer is empty/invalid", env.valid_request, &empty_response,
       SRV_EDHOC_MSG1_HDL_ERR_INVALID_RESPONSE_BUFFER},
      {"credentials are missing", empty_request, &env.response,
       SRV_EDHOC_MSG1_HDL_ERR_NULL_CREDENTIALS},
  };

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    tst_srv_edhoc_m1_reset_stub_results();
    const struct srv_edhoc_message_1_handler_result result =
        srv_edhoc_handle_message_1(test_cases[i].request,
                                   test_cases[i].response);

    TEST_ASSERT_EQUAL_MESSAGE(test_cases[i].expected_status, result.status,
                              test_cases[i].description);

    if (test_cases[i].response != NULL &&
        test_cases[i].response->bytes != NULL) {
      tst_srv_edhoc_assert_handler_writes_error_in_buffer(
          *test_cases[i].response);
    }
    ensure_context_is_freed_on_failure(result);
  }
}

void test_handler_fails_on_library_errors(void) {
  const struct {
    const char* description;
    void (*setup_scenario)(void);
    enum srv_edhoc_message_1_handler_status expected_status;
  } cases[] = {
      {"setup fails", tst_srv_edhoc_m1_set_setup_failure,
       SRV_EDHOC_MSG1_HDL_ERR_EDHOC_CONTEXT_SETUP_FAILED},
      {"processing fails", tst_srv_edhoc_m1_set_message_1_process_failure,
       SRV_EDHOC_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED},
      {"composition fails", tst_srv_edhoc_m1_set_message_2_compose_failure,
       SRV_EDHOC_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED},
  };

  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
    tst_srv_edhoc_m1_reset_stub_results();
    cases[i].setup_scenario();

    const struct srv_edhoc_message_1_handler_result result =
        srv_edhoc_handle_message_1(env.valid_request, &env.response);

    TEST_ASSERT_EQUAL_MESSAGE(cases[i].expected_status, result.status,
                              cases[i].description);
  }
}

void test_handler_fails_when_message_2_composition_produces_empty_buffer(void) {
  tst_srv_edhoc_m1_set_message_2_compose_empty_length();

  const struct srv_edhoc_message_1_handler_result result =
      srv_edhoc_handle_message_1(env.valid_request, &env.response);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_EMPTY,
                    result.status);
  tst_srv_edhoc_assert_handler_writes_error_in_buffer(env.response);
  ensure_context_is_freed_on_failure(result);
}