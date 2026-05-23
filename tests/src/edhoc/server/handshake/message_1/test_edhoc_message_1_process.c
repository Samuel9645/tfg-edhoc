
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

#include "edhoc/common/add_error/common/tst_edhoc_add_error_capacity.h"
#include "edhoc/common/add_error/mocks/tst_mock_edhoc_get_code.h"
#include "edhoc/common/com_edhoc_cipher_suites.h"
#include "edhoc/common/tst_edhoc_params.h"
#include "edhoc/server/handshake/message_1/srv_m1_process.h"
#include "edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_context_init.h"
#include "edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_message_1_process.h"

enum { TST_SRV_EDHOC_HND_BUF_LEN = 256 };

static const uint8_t DUMMY_REQUEST_BUFFER[TST_SRV_EDHOC_HND_BUF_LEN] = {0};

static struct {
  uint8_t response_buffer[TST_SRV_EDHOC_HND_BUF_LEN];
  struct com_writable_buffer error;
} env = {.error = {.capacity = TST_SRV_EDHOC_HND_BUF_LEN}};

static void reset_mock_results(void) {
  tst_srv_edhoc_m1_reset_process_mock();
  tst_srv_edhoc_m1_reset_context_init_mock();
  tst_com_edhoc_reset_get_code_mock();
}

// TODO: this is duplicated with m1 responder
static struct srv_edhoc_message_1_request create_valid_request(void) {
  return (struct srv_edhoc_message_1_request){
      .payload = {.bytes = DUMMY_REQUEST_BUFFER,
                  .length = TST_SRV_EDHOC_HND_BUF_LEN}};
}

void setUp(void) {
  reset_mock_results();
  memset(env.response_buffer, 0, sizeof(env.response_buffer));
  env.error.bytes = env.response_buffer;
}

static void ensure_context_is_null(struct edhoc_context* context) {
  TEST_ASSERT_NULL_MESSAGE(context, "edhoc context was not NULL");
}

static struct srv_edhoc_message_1_request create_msg1_request(
    const uint8_t* payload, const size_t length) {
  return (struct srv_edhoc_message_1_request){
      .payload = {.bytes = payload, .length = length}};
}

static void use_real_implementations(void) {
  tst_com_edhoc_use_real_get_code();
}

/**
 * @see [RFC
 * 9529 2.1](https://datatracker.ietf.org/doc/html/rfc9529#name-message_1)
 */
void test_m1_process_ok_for_valid_data(void) {
  // Extracted from RFC trace
  const uint8_t MESSAGE_1_SUITE_0_METHOD_0[] = {
      0x00, 0x00, 0x58, 0x20, 0x31, 0xf8, 0x2c, 0x7b, 0x5b, 0x9c,
      0xbb, 0xf0, 0xf1, 0x94, 0xd9, 0x13, 0xcc, 0x12, 0xef, 0x15,
      0x32, 0xd3, 0x28, 0xef, 0x32, 0x63, 0x2a, 0x48, 0x81, 0xa1,
      0xc0, 0x70, 0x1e, 0x23, 0x7f, 0x04, 0x2d};
  tst_com_edhoc_use_real_get_code();
  const struct srv_edhoc_message_1_request request = create_msg1_request(
      MESSAGE_1_SUITE_0_METHOD_0, sizeof(MESSAGE_1_SUITE_0_METHOD_0));

  struct srv_edhoc_message_1_process_result result =
      srv_edhoc_process_message_1(request, env.error,
                                  tst_edhoc_get_method_0_suite_0_params());

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG1_PROCESS_OK, result.status);
  TEST_ASSERT_NOT_NULL(result.context);
  const enum srv_edhoc_cleanup_context_status cleanup_status =
      srv_edhoc_cleanup_context(result.context);
  TEST_ASSERT_EQUAL_MESSAGE(SRV_EDHOC_CLEANUP_OK, cleanup_status,
                            "edhoc_context cleanup failed");
}

static void assert_error_contains_supported_cipher_suites(
    const struct com_readonly_buffer encoded_error_buffer,
    const struct com_edhoc_cipher_suite_list expected_cipher_suites) {
  enum edhoc_error_code received_code = -1;
  char decoded_error[TST_EDHOC_ADD_ERROR_CAPACITY] = {0};
  struct edhoc_error_info received_info = {
      .text_string = decoded_error, .total_entries = sizeof(decoded_error)};
  const int process_status = edhoc_message_error_process(
      encoded_error_buffer.bytes, encoded_error_buffer.length, &received_code,
      &received_info);
  TEST_ASSERT_EQUAL_INT_MESSAGE(EDHOC_SUCCESS, process_status,
                                "CBOR Decode Failed");
  TEST_ASSERT_EQUAL_MESSAGE(EDHOC_ERROR_CODE_WRONG_SELECTED_CIPHER_SUITE,
                            received_code, "Unexpected error code");
  TEST_ASSERT_EQUAL_MESSAGE(expected_cipher_suites.number_of_suites,
                            received_info.written_entries,
                            "Number of suites do not match");
  for (size_t i = 0; i < expected_cipher_suites.number_of_suites; i++) {
    TEST_ASSERT_EQUAL(expected_cipher_suites.suites[i]->metadata->value,
                      received_info.cipher_suites[i]);
  }
}

/**
 * @see [RFC
 * 9528 6.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-wrong-selected-cipher-suite)
 * @see [RFC 9529
 * 3](https://datatracker.ietf.org/doc/html/rfc9529#name-authentication-with-static-)
 */
void test_m1_process_reports_cipher_suite_mismatch(void) {
  // Extracted from RFC 9529 3
  const uint8_t MESSAGE_1_SUITE_6_REQUEST[] = {
      0x03, 0x06, 0x58, 0x20, 0x74, 0x1a, 0x13, 0xd7, 0xba, 0x04,
      0x8f, 0xbb, 0x61, 0x5e, 0x94, 0x38, 0x6a, 0xa3, 0xb6, 0x1b,
      0xea, 0x5b, 0x3d, 0x8f, 0x65, 0xf3, 0x26, 0x20, 0xb7, 0x49,
      0xbe, 0xe8, 0xd2, 0x78, 0xef, 0xa9, 0x0e};
  const struct srv_edhoc_message_1_request request = create_msg1_request(
      MESSAGE_1_SUITE_6_REQUEST, sizeof(MESSAGE_1_SUITE_6_REQUEST));
  use_real_implementations();

  const enum edhoc_method ONLY_METHOD_3[] = {EDHOC_METHOD_3};
  const struct com_edhoc_parameters method_3_suite_2_params =
      tst_edhoc_create_test_params(
          COM_EDHOC_ONLY_SUITE_2,
          (struct com_edhoc_methods){
              .data = ONLY_METHOD_3,
              .size = sizeof(ONLY_METHOD_3) / sizeof(ONLY_METHOD_3[0])},
          &COM_EDHOC_SUITE_2);

  const struct srv_edhoc_message_1_process_result result =
      srv_edhoc_process_message_1(request, env.error, method_3_suite_2_params);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG1_PROCESS_ERR_EDHOC_PROCESS, result.status);
  assert_error_contains_supported_cipher_suites(
      result.error_buffer, method_3_suite_2_params.supported_cipher_suites);
}

void test_m1_process_fails_on_invalid_data(void) {
  const struct srv_edhoc_message_1_request valid_request =
      create_valid_request();
  const struct srv_edhoc_message_1_request empty_request = {0};
  const struct com_writable_buffer empty_response = {0};

  const struct {
    const char* description;
    struct srv_edhoc_message_1_request request;
    struct com_writable_buffer response;
    enum srv_edhoc_message_1_process_status expected_status;
  } test_cases[] = {
      {"empty request payload", empty_request, env.error,
       SRV_EDHOC_MSG1_PROCESS_ERR_EMPTY_REQUEST_BUFFER},
      {"error buffer is empty/invalid", valid_request, empty_response,
       SRV_EDHOC_MSG1_PROCESS_ERR_INVALID_ERROR_BUFFER}};

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    reset_mock_results();
    const struct srv_edhoc_message_1_process_result result =
        srv_edhoc_process_message_1(test_cases[i].request,
                                    test_cases[i].response,
                                    tst_edhoc_get_method_0_suite_0_params());

    TEST_ASSERT_EQUAL_MESSAGE(test_cases[i].expected_status, result.status,
                              test_cases[i].description);
    ensure_context_is_null(result.context);
  }
}

void test_m1_process_fails_on_library_errors(void) {
  const struct {
    const char* description;
    void (*setup_scenario)(void);
    enum srv_edhoc_message_1_process_status expected_status;
  } cases[] = {
      {NULL, tst_srv_edhoc_m1_set_context_init_failure,
       SRV_EDHOC_MSG1_PROCESS_ERR_EDHOC_CONTEXT_SETUP},
      {"Message 1 Process error: Processing failed",
       tst_srv_edhoc_m1_set_process_failure,
       SRV_EDHOC_MSG1_PROCESS_ERR_EDHOC_PROCESS},
  };

  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
    reset_mock_results();
    cases[i].setup_scenario();

    const struct srv_edhoc_message_1_process_result result =
        srv_edhoc_process_message_1(create_valid_request(), env.error,
                                    tst_edhoc_get_method_0_suite_0_params());

    TEST_ASSERT_EQUAL_MESSAGE(cases[i].expected_status, result.status,
                              cases[i].description);
  }
}
