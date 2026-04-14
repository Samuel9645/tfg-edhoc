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

#include <edhoc.h>
#include <string.h>
#include <unity.h>

#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"
#include "edhoc/server/handshake/message_3/handler/tst_srv_mock_m3_handler_deps.h"
#include "edhoc/server/handshake/message_3/srv_m3_handler.h"
#include "edhoc/server/handshake/message_3/srv_m3_result.h"

enum { TST_SRV_EDHOC_HND_BUF_LEN = 256 };

static const uint8_t REQUEST_BUFFER[TST_SRV_EDHOC_HND_BUF_LEN] = {0};

static struct {
  uint8_t response_buffer[TST_SRV_EDHOC_HND_BUF_LEN];
  struct edhoc_context context;
  struct srv_edhoc_message_3_request valid_request;
  struct com_writable_buffer response;
} env = {
    .response = {.capacity = TST_SRV_EDHOC_HND_BUF_LEN},
    .valid_request = {.parsed_message_3 = {.bytes = REQUEST_BUFFER,
                                           .length = sizeof(REQUEST_BUFFER)}}};

void setUp(void) {
  tst_srv_edhoc_m3_reset_stub_results();
  memset(env.response_buffer, 0, sizeof(env.response_buffer));
  env.response.bytes = env.response_buffer;
  env.response.length = 0;
  env.valid_request.edhoc_context = &env.context;
  env.context = (struct edhoc_context){0};
}

void test_handler_ok_on_valid_data(void) {
  const enum srv_edhoc_message_3_handler_status status =
      srv_edhoc_handle_message_3(env.valid_request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG3_HDL_OK, status);
  tst_srv_edhoc_m3_assert_handler_writes_message_4_in_buffer(env.response);
}

void test_handler_fails_on_invalid_data(void) {
  const struct srv_edhoc_message_3_request no_context = {
      .parsed_message_3 = env.valid_request.parsed_message_3};
  const struct srv_edhoc_message_3_request no_buffer = {.edhoc_context =
                                                            &env.context};
  const struct srv_edhoc_message_3_request empty_request = {
      .edhoc_context = &env.context,
      .parsed_message_3 = {.bytes = REQUEST_BUFFER, .length = 0}};
  struct com_writable_buffer unwritable_response = {
      .bytes = env.response.bytes,
      .capacity = 0,
      .length = 0,
  };

  const struct {
    const char* description;
    struct srv_edhoc_message_3_request request;
    struct com_writable_buffer* response;
    enum srv_edhoc_message_3_handler_status expected;
    bool should_check_error_payload;
  } test_cases[] = {
      {"response is NULL", env.valid_request, NULL,
       EDH_MSG3_HDL_ERR_INVALID_RESPONSE_BUFFER, false},
      {"response has 0 capacity", env.valid_request, &unwritable_response,
       EDH_MSG3_HDL_ERR_INVALID_RESPONSE_BUFFER, false},
      {"context is NULL", no_context, &env.response,
       EDH_MSG3_HDL_ERR_NULL_EDHOC_CONTEXT, true},
      {"request buffer is NULL", no_buffer, &env.response,
       EDH_MSG3_HDL_ERR_INVALID_PARSED_MESSAGE_3, true},
      {"request payload is empty (len 0)", empty_request, &env.response,
       EDH_MSG3_HDL_ERR_INVALID_PARSED_MESSAGE_3, true}};

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    tst_srv_edhoc_m3_reset_stub_results();
    const enum srv_edhoc_message_3_handler_status status =
        srv_edhoc_handle_message_3(test_cases[i].request,
                                   test_cases[i].response);

    TEST_ASSERT_EQUAL_MESSAGE(test_cases[i].expected, status,
                              test_cases[i].description);
    if (test_cases[i].should_check_error_payload) {
      tst_edhoc_assert_encoded_error_is_not_empty(env.response);
    }
  }
}

void test_handler_fails_when_message_3_processing_fails(void) {
  tst_srv_edhoc_m1_set_message_3_process_failure();

  const enum srv_edhoc_message_3_handler_status status =
      srv_edhoc_handle_message_3(env.valid_request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG3_HDL_ERR_MESSAGE_3_PROCESS_FAILED, status);
  tst_edhoc_assert_encoded_error_is_not_empty(env.response);
}

void test_handler_fails_when_message_4_composition_fails(void) {
  tst_srv_edhoc_m1_set_message_4_compose_failure();

  const enum srv_edhoc_message_3_handler_status status =
      srv_edhoc_handle_message_3(env.valid_request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_FAILED, status);
  tst_edhoc_assert_encoded_error_is_not_empty(env.response);
}

void test_handler_fails_when_message_4_composition_produces_empty_buffer(void) {
  tst_srv_edhoc_m3_set_message_4_compose_empty_length();

  const enum srv_edhoc_message_3_handler_status status =
      srv_edhoc_handle_message_3(env.valid_request, &env.response);

  TEST_ASSERT_EQUAL(EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_EMPTY, status);
  tst_edhoc_assert_encoded_error_is_not_empty(env.response);
}
