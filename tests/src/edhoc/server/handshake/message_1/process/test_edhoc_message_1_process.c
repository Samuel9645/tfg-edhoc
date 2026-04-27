
/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 16/04/2026
 * @brief Tests for Message 1 Processing function
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
#include "edhoc/common/tst_edhoc_default_params.h"
#include "edhoc/edhoc_config.h"
#include "edhoc/server/handshake/message_1/srv_m1_process.h"
#include "edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_context_init.h"
#include "edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_message_1_process.h"

enum { TST_SRV_EDHOC_HND_BUF_LEN = 256 };

static const uint8_t REQUEST_BUFFER[TST_SRV_EDHOC_HND_BUF_LEN] = {0};

static struct {
  uint8_t response_buffer[TST_SRV_EDHOC_HND_BUF_LEN];
  struct com_writable_buffer error;
} env = {.error = {.capacity = TST_SRV_EDHOC_HND_BUF_LEN}};

static void reset_mock_results(void) {
  tst_srv_edhoc_m1_reset_process_mock();
  tst_srv_edhoc_m1_reset_context_init_mock();
}

// TODO: this is duplicated with m1 responder
static struct srv_edhoc_message_1_request create_valid_request(void) {
  return (struct srv_edhoc_message_1_request){
      .payload = {.bytes = REQUEST_BUFFER, .length = TST_SRV_EDHOC_HND_BUF_LEN},
      .edhoc_parameters = tst_edhoc_srv_get_default_params()};
}

void setUp(void) {
  reset_mock_results();
  memset(env.response_buffer, 0, sizeof(env.response_buffer));
  env.error.bytes = env.response_buffer;
}

static void ensure_context_is_null(struct edhoc_context* context) {
  TEST_ASSERT_NULL_MESSAGE(context, "edhoc context was not NULL");
}

void test_m1_process_ok_for_valid_data(void) {
  tst_srv_edhoc_m1_set_process_ok();

  struct srv_edhoc_message_1_process_result result =
      srv_edhoc_process_message_1(create_valid_request(), env.error);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG1_PROCESS_OK, result.status);
  TEST_ASSERT_NOT_NULL(result.context);
  const enum srv_edhoc_cleanup_context_status cleanup_status =
      srv_edhoc_cleanup_context(&result.context);
  TEST_ASSERT_EQUAL_MESSAGE(SRV_EDHOC_CLEANUP_OK, cleanup_status,
                            "edhoc_context cleanup failed");
  ensure_context_is_null(result.context);
}

void test_m1_process_fails_on_invalid_data(void) {
  const struct srv_edhoc_message_1_request valid_request =
      create_valid_request();
  const struct srv_edhoc_message_1_request valid_request_without_credentials = {
      .payload = valid_request.payload,
      .edhoc_parameters = {
          .credentials = NULL,
          .supported_cipher_suites =
              valid_request.edhoc_parameters.supported_cipher_suites}};
  const struct srv_edhoc_message_1_request valid_request_without_suites = {
      .payload = valid_request.payload,
      .edhoc_parameters = {
          .supported_cipher_suites = NULL,
          .credentials = valid_request.edhoc_parameters.credentials}};
  const struct srv_edhoc_message_1_request empty_request_with_edhoc_params = {
      .edhoc_parameters = valid_request.edhoc_parameters};
  const struct com_writable_buffer empty_response = {0};

  const struct {
    const char* description;
    struct srv_edhoc_message_1_request request;
    struct com_writable_buffer response;
    enum srv_edhoc_message_1_process_status expected_status;
  } test_cases[] = {
      {"empty request payload", empty_request_with_edhoc_params, env.error,
       SRV_EDHOC_MSG1_PROCESS_ERR_EMPTY_REQUEST_BUFFER},
      {"error buffer is empty/invalid", valid_request, empty_response,
       SRV_EDHOC_MSG1_PROCESS_ERR_INVALID_ERROR_BUFFER},
      {"credentials are missing", valid_request_without_credentials, env.error,
       SRV_EDHOC_MSG1_PROCESS_ERR_NULL_CREDENTIALS},
      {"invalid cipher suites", valid_request_without_suites, env.error,
       SRV_EDHOC_MSG1_PROCESS_ERR_INVALID_CIPHER_SUITES}};

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    reset_mock_results();
    const struct srv_edhoc_message_1_process_result result =
        srv_edhoc_process_message_1(test_cases[i].request,
                                    test_cases[i].response);

    TEST_ASSERT_EQUAL_MESSAGE(test_cases[i].expected_status, result.status,
                              test_cases[i].description);

    tst_edhoc_assert_error_not_empty_if_present(result.error_buffer);
    ensure_context_is_null(result.context);
  }
}

/**
 * WHY DON'T WE CHECK THE REPORTED ERROR?
 *
 * RFC only defines specific errors for other scenarios, so most of the RFC
 * level codes are UNSPECIFIED_ERROR.
 * @see [RFC 9528, Section
 * 6](https://datatracker.ietf.org/doc/html/rfc9528/#name-error-handling)
 */
void test_m1_process_fails_on_library_errors(void) {
  const struct {
    const char* description;
    void (*setup_scenario)(void);
    enum srv_edhoc_message_1_process_status expected_status;
  } cases[] = {
      {"setup fails", tst_srv_edhoc_m1_set_context_init_failure,
       SRV_EDHOC_MSG1_PROCESS_ERR_EDHOC_CONTEXT_SETUP},
      {"processing fails", tst_srv_edhoc_m1_set_process_failure,
       SRV_EDHOC_MSG1_PROCESS_ERR_EDHOC_PROCESS},
  };

  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
    reset_mock_results();
    cases[i].setup_scenario();

    const struct srv_edhoc_message_1_process_result result =
        srv_edhoc_process_message_1(create_valid_request(), env.error);

    TEST_ASSERT_EQUAL_MESSAGE(cases[i].expected_status, result.status,
                              cases[i].description);
    tst_edhoc_assert_encoded_error_is_not_empty(result.error_buffer);
  }
}
