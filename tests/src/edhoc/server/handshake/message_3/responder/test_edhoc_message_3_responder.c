/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Tests for Message 3 responder.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include <edhoc.h>
#include <string.h>
#include <unity.h>

#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"
#include "edhoc/server/handshake/message_3/srv_m3_responder.h"
#include "edhoc/server/handshake/mocks/message_3/tst_srv_mock_edhoc_message_3_process.h"
#include "edhoc/server/handshake/mocks/message_3/tst_srv_mock_m3_parser_deps.h"
#include "edhoc/server/handshake/mocks/message_4/tst_srv_mock_edhoc_message_4_compose.h"

enum { TST_SRV_EDHOC_HND_BUF_LEN = 256 };

static const uint8_t DUMMY_REQUEST_BUFFER[] = {0x01, 0x02, 0x03, 0x04,
                                               0x05, 0x06, 0x07, 0x08};

static struct {
  uint8_t response_buffer[TST_SRV_EDHOC_HND_BUF_LEN];
  struct edhoc_context context;
  struct srv_edhoc_message_3_responder_request valid_request;
  struct com_writable_buffer response;
} env = {.response = {.capacity = TST_SRV_EDHOC_HND_BUF_LEN},
    .valid_request = {.raw_payload = {.bytes = DUMMY_REQUEST_BUFFER,
                                      .length = sizeof(DUMMY_REQUEST_BUFFER)}}};

static void reset_mocks(void) {
  tst_srv_edhoc_message_3_parser_reset_stub_results();
  tst_srv_edhoc_m3_reset_process_mock();
  tst_srv_edhoc_m4_reset_compose_mock();
  tst_srv_m3_parser_set_success_data((struct edhoc_extracted_fields){
      .buffer = DUMMY_REQUEST_BUFFER,
      .buffer_size = sizeof(DUMMY_REQUEST_BUFFER),
      .edhoc_message_ptr = DUMMY_REQUEST_BUFFER + 1,
      .edhoc_message_size = sizeof(DUMMY_REQUEST_BUFFER) - 1,
  });
}

void setUp(void) {
  reset_mocks();
  memset(env.response_buffer, 0, sizeof(env.response_buffer));
  env.response.bytes = env.response_buffer;
  env.context = (struct edhoc_context){0};
  env.valid_request.edhoc_context = &env.context;
}

void test_responder_ok_for_valid_data(void) {
  tst_srv_edhoc_m3_set_process_ok();
  tst_srv_edhoc_m4_set_compose_ok();

  const struct srv_edhoc_message_3_responder_result result =
      srv_edhoc_respond_to_message_3(env.valid_request, env.response);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_RESPONDER_OK, result.status);
  tst_srv_edhoc_m4_compose_assert_writes_message_in_buffer(result.response);
}

void test_responder_fails_on_invalid_data(void) {
  const struct com_writable_buffer empty_response = {0};
  const struct srv_edhoc_message_3_responder_request no_buffer_request = {
      .raw_payload = {.bytes = NULL, .length = sizeof(DUMMY_REQUEST_BUFFER)},
      .edhoc_context = &env.context,
  };
  const struct srv_edhoc_message_3_responder_request empty_payload_request = {
      .raw_payload = {.bytes = DUMMY_REQUEST_BUFFER, .length = 0},
      .edhoc_context = &env.context,
  };
  const struct srv_edhoc_message_3_responder_request no_context_request = {
      .raw_payload = {.bytes = DUMMY_REQUEST_BUFFER,
                      .length = sizeof(DUMMY_REQUEST_BUFFER)},
      .edhoc_context = NULL};

  const struct {
    const char* description;
    struct srv_edhoc_message_3_responder_request request;
    struct com_writable_buffer response;
    enum srv_edhoc_message_3_responder_status expected;
  } test_cases[] = {
      {"response has 0 capacity", env.valid_request, empty_response,
       SRV_EDHOC_MSG3_RESPONDER_ERR_INVALID_RESPONSE_BUFFER},
      {"request buffer is NULL", no_buffer_request, env.response,
       SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_3_PARSE},
      {"request payload is empty (len 0)", empty_payload_request, env.response,
       SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_3_PARSE},
      {"context is NULL", no_context_request, env.response,
       SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_3_PARSE},
  };

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    reset_mocks();

    const struct srv_edhoc_message_3_responder_result result =
        srv_edhoc_respond_to_message_3(test_cases[i].request,
                                       test_cases[i].response);

    TEST_ASSERT_EQUAL_MESSAGE(test_cases[i].expected, result.status,
                              test_cases[i].description);
    tst_edhoc_assert_error_not_empty_if_present(result.response);
  }
}

void test_responder_fails_when_parser_reports_extraction_failure(void) {
  tst_srv_m3_parser_set_extraction_failure();

  const struct srv_edhoc_message_3_responder_result result =
      srv_edhoc_respond_to_message_3(env.valid_request, env.response);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_3_PARSE,
                    result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.response);
}

void test_responder_fails_when_parser_reports_connection_id_mismatch(void) {
  tst_srv_m3_parser_set_cid_mismatch();

  const struct srv_edhoc_message_3_responder_result result =
      srv_edhoc_respond_to_message_3(env.valid_request, env.response);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_3_PARSE,
                    result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.response);
}

/**
 * WHY DON'T WE CHECK THE REPORTED ERROR?
 *
 * RFC only defines specific errors for other scenarios, so most of the RFC
 * level codes are UNSPECIFIED_ERROR.
 * @see [RFC 9528, Section
 * 6](https://datatracker.ietf.org/doc/html/rfc9528/#name-error-handling)
 */
void test_responder_fails_on_library_errors(void) {
  const struct {
    const char* description;
    void (*setup_scenario)(void);
    enum srv_edhoc_message_3_responder_status expected_status;
  } cases[] = {
      {"message 3 processing fails", tst_srv_edhoc_m3_set_process_failure,
       SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_3_PROCESS},
      {"message 4 composition fails", tst_srv_edhoc_m4_set_compose_failure,
       SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_4_COMPOSE},
      {"message 4 composition produces empty buffer",
       tst_srv_edhoc_m4_compose_set_compose_empty_length,
       SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_4_COMPOSE},
  };

  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
    reset_mocks();
    cases[i].setup_scenario();

    const struct srv_edhoc_message_3_responder_result result =
        srv_edhoc_respond_to_message_3(env.valid_request, env.response);

    TEST_ASSERT_EQUAL_MESSAGE(cases[i].expected_status, result.status,
                              cases[i].description);
    tst_edhoc_assert_encoded_error_is_not_empty(result.response);
  }
}
