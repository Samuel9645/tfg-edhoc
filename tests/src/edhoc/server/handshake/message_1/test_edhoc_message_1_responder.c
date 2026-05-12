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

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include <coap3/coap.h>
#include <edhoc.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"
#include "edhoc/common/tst_edhoc_params.h"
#include "edhoc/server/handshake/message_1/srv_m1_responder.h"
#include "edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_extract_flow_info.h"
#include "edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_message_1_process.h"
#include "edhoc/server/handshake/mocks/message_2/tst_srv_mock_edhoc_message_2_compose.h"

enum { TST_SRV_EDHOC_HND_BUF_LEN = 256 };

// WITH PREFIX 0xF5 so that parsing succeeds
static const uint8_t DUMMY_REQUEST_BUFFER[] = {0xF5, 0x02, 0x03, 0x04,
                                               0x05, 0x06, 0x07, 0x08};

static struct {
  uint8_t response_buffer[TST_SRV_EDHOC_HND_BUF_LEN];
  struct com_writable_buffer response;
} env = {.response = {.capacity = TST_SRV_EDHOC_HND_BUF_LEN}};

static void reset_mocks(void) {
  tst_srv_edhoc_m1_reset_process_mock();
  tst_srv_edhoc_m2_reset_compose_mock();
}

void setUp(void) {
  reset_mocks();
  memset(env.response_buffer, 0, sizeof(env.response_buffer));
  env.response.bytes = env.response_buffer;
}

static struct srv_edhoc_message_1_responder_request create_valid_request(void) {
  return (struct srv_edhoc_message_1_responder_request){
      .message_1 = {.bytes = DUMMY_REQUEST_BUFFER,
                    .length = TST_SRV_EDHOC_HND_BUF_LEN}};
}

void test_responder_ok_for_valid_data(void) {
  tst_srv_edhoc_m1_set_process_ok();
  tst_srv_edhoc_m2_set_compose_ok();

  const struct srv_edhoc_message_1_responder_result result =
      srv_edhoc_respond_to_message_1(create_valid_request(),
                                     tst_edhoc_get_method_0_suite_0_params(),
                                     env.response);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG1_RESPONDER_OK, result.status);
  tst_srv_edhoc_m2_compose_assert_writes_message_in_buffer(result.response);
  TEST_ASSERT_NOT_NULL(result.edhoc_ctx);
  free(result.edhoc_ctx);
}

static void ensure_context_is_freed_on_failure(
    const struct srv_edhoc_message_1_responder_result result) {
  TEST_ASSERT_NULL_MESSAGE(result.edhoc_ctx,
                           "Memory Leak: edhoc_ctx was not NULL on failure");
}

/**
 * @see [RFC
 * 9529 3.5](https://datatracker.ietf.org/doc/html/rfc9529#name-message_3-2)
 * @see [RFC
 * 9528 5.4](https://datatracker.ietf.org/doc/html/rfc9528/#name-edhoc-message-3)
 */
void test_responder_reports_failure_when_receiving_message_3(void) {}

void test_responder_fails_on_invalid_data(void) {
  const struct com_writable_buffer empty_response = {0};
  const struct com_edhoc_parameters valid_parameters =
      tst_edhoc_get_method_0_suite_0_params();
  const struct srv_edhoc_message_1_responder_request valid_request =
      create_valid_request();
  const struct srv_edhoc_message_1_responder_request empty_payload_request = {
      .message_1 = {.bytes = NULL, .length = 0}};
  const struct com_edhoc_parameters empty_params = {0};

  const struct {
    const char* description;
    struct srv_edhoc_message_1_responder_request request;
    struct com_edhoc_parameters edhoc_parameters;
    struct com_writable_buffer response;
  } test_cases[] = {
      {"response buffer is empty/invalid", valid_request, valid_parameters,
       empty_response},
      {"raw payload is empty", empty_payload_request, valid_parameters,
       env.response},
      {"empty params", valid_request, empty_params, env.response},
  };

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    reset_mocks();
    const struct srv_edhoc_message_1_responder_result result =
        srv_edhoc_respond_to_message_1(test_cases[i].request,
                                       test_cases[i].edhoc_parameters,
                                       test_cases[i].response);

    TEST_ASSERT_NOT_EQUAL_MESSAGE(SRV_EDHOC_MSG1_RESPONDER_OK, result.status,
                                  test_cases[i].description);
    tst_edhoc_assert_error_not_empty_if_present(result.response);
    ensure_context_is_freed_on_failure(result);
  }
}

static void only_m2_compose_failure(void) {
  tst_srv_edhoc_m1_set_process_ok();
  tst_srv_edhoc_m2_set_compose_failure();
}

static void only_m2_compose_empty_but_ok(void) {
  tst_srv_edhoc_m1_set_process_ok();
  tst_srv_edhoc_m2_compose_set_compose_empty_length();
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
    enum srv_edhoc_message_1_responder_status expected_status;
  } cases[] = {
      {"setup fails", tst_srv_edhoc_m1_set_process_failure,
       SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_1_PROCESS_FAILED},
      {"composition fails", only_m2_compose_failure,
       SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_2_COMPOSE_FAILED},
      {"composition produces empty buffer", only_m2_compose_empty_but_ok,
       SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_2_COMPOSE_FAILED},
  };

  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
    reset_mocks();
    cases[i].setup_scenario();

    const struct srv_edhoc_message_1_responder_result result =
        srv_edhoc_respond_to_message_1(create_valid_request(),
                                       tst_edhoc_get_method_0_suite_0_params(),
                                       env.response);

    TEST_ASSERT_EQUAL_MESSAGE(cases[i].expected_status, result.status,
                              cases[i].description);
    tst_edhoc_assert_encoded_error_is_not_empty(result.response);
    ensure_context_is_freed_on_failure(result);
  }
}