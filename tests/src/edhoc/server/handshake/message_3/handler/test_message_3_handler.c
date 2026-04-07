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

#include "edhoc/server/handshake/message_3/handler/tst_m3_hdl_helpers.h"
#include "edhoc/server/handshake/message_3/handler/tst_srv_m3_stubs.h"

static struct tst_edh_srv_message_3_env env = {0};

void setUp(void) {
  tst_edh_srv_message_3_reset_stub_results();
  memset(&env, 0, sizeof(env));
  tst_edh_srv_message_3_setup_env(&env);
}

void test_handler_fails_on_invalid_data(void) {
  const struct edh_srv_message_3_request no_context =
      tst_message_3_request_without_context(&env);
  const struct edh_srv_message_3_request no_buffer =
      tst_message_3_request_without_buffer(&env);

  const struct {
    const char* description;
    struct edh_srv_message_3_request request;
    struct com_writable_buffer* response;
    enum edh_srv_message_3_handler_status expected;
  } test_cases[] = {{"response is missing", env.request, NULL,
                     EDH_MSG3_HDL_ERR_INVALID_RESPONSE_BUFFER},
                    {"context is missing", no_context, &env.response,
                     EDH_MSG3_HDL_ERR_NULL_EDHOC_CONTEXT},
                    {"buffer is missing", no_buffer, &env.response,
                     EDH_MSG3_HDL_ERR_INVALID_PARSED_MESSAGE_3}};

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    const enum edh_srv_message_3_handler_status status =
        edh_srv_handle_message_3(test_cases[i].request, test_cases[i].response);

    TEST_ASSERT_EQUAL_MESSAGE(test_cases[i].expected, status,
                              test_cases[i].description);
  }
}

void test_handler_fails_when_message_3_processing_fails(void) {
  tst_edh_srv_stub_process_message_3_result = EDHOC_ERROR_CRYPTO_FAILURE;

  const enum edh_srv_message_3_handler_status status =
      edh_srv_handle_message_3(env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG3_HDL_ERR_MESSAGE_3_PROCESS_FAILED, status);
  tst_edh_srv_m3_assert_handler_writes_error_payload(env.response);
}

void test_handler_fails_when_message_4_composition_fails(void) {
  tst_edh_srv_stub_message_4_compose_result = EDHOC_ERROR_BUFFER_TOO_SMALL;

  const enum edh_srv_message_3_handler_status status =
      edh_srv_handle_message_3(env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_FAILED, status);
  tst_edh_srv_m3_assert_handler_writes_error_payload(env.response);
}

void test_handler_fails_when_message_4_composition_produces_empty_buffer(void) {
  tst_edh_srv_stub_message_4_compose_result = EDHOC_SUCCESS;
  tst_edh_srv_stub_message_4_compose_written_length = 0;

  const enum edh_srv_message_3_handler_status status =
      edh_srv_handle_message_3(env.request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_EMPTY, status);
}
