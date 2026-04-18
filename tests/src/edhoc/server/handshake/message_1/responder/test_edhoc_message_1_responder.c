
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
#include "edhoc/edhoc_config.h"
#include "edhoc/server/handshake/message_1/srv_m1_responder.h"
#include "edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_message_1_process.h"
#include "edhoc/server/handshake/mocks/message_2/tst_srv_mock_edhoc_message_2_compose.h"

enum { TST_SRV_EDHOC_HND_BUF_LEN = 256 };

static int dummy_fetch_credentials(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  (void)user_context;
  (void)credentials;
  return EDHOC_SUCCESS;
}

static int dummy_verify_credentials(
    void* user_context, struct edhoc_auth_creds* credentials,
    const uint8_t** public_key_reference,
    size_t* public_key_length) {  // NOLINT(*-non-const-parameter)
  (void)user_context;
  (void)credentials;
  (void)public_key_reference;
  (void)public_key_length;
  return EDHOC_SUCCESS;
}

static const struct edhoc_credentials DUMMY_TEST_CREDS = {
    .fetch = dummy_fetch_credentials, .verify = dummy_verify_credentials};
static const uint8_t REQUEST_BUFFER[TST_SRV_EDHOC_HND_BUF_LEN] = {0};

static struct {
  uint8_t response_buffer[TST_SRV_EDHOC_HND_BUF_LEN];
  const struct srv_edhoc_message_1_request valid_request;
  struct com_writable_buffer response;
} env = {.response = {.capacity = TST_SRV_EDHOC_HND_BUF_LEN},
         .valid_request = {.payload = {.length = TST_SRV_EDHOC_HND_BUF_LEN,
                                       .bytes = REQUEST_BUFFER},
                           .credentials = &DUMMY_TEST_CREDS}};

static void reset_mocks(void) {
  tst_srv_edhoc_m1_reset_process_mock();
  tst_srv_edhoc_m2_reset_compose_mock();
}

void setUp(void) {
  reset_mocks();
  memset(env.response_buffer, 0, sizeof(env.response_buffer));
  env.response.bytes = env.response_buffer;
  env.response.length = 0;
}

void test_handler_ok_for_valid_data(void) {
  tst_srv_edhoc_m2_set_compose_ok();

  const struct srv_edhoc_message_1_responder_result result =
      srv_edhoc_respond_to_message_1(env.valid_request, &env.response);

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
    enum srv_edhoc_message_1_responder_status expected_status;
  } test_cases[] = {
      {"response buffer is NULL", env.valid_request, NULL,
       SRV_EDHOC_MSG1_RESPONDER_ERR_INVALID_RESPONSE_BUFFER},
      {"response buffer is empty/invalid", env.valid_request, &empty_response,
       SRV_EDHOC_MSG1_RESPONDER_ERR_INVALID_RESPONSE_BUFFER},
      {"empty request payload", empty_request_with_credentials, &env.response,
       SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_1_PROCESS_FAILED},
      {"credentials are missing", empty_request, &env.response,
       SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_1_PROCESS_FAILED},
  };

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    reset_mocks();
    const struct srv_edhoc_message_1_responder_result result =
        srv_edhoc_respond_to_message_1(test_cases[i].request,
                                       test_cases[i].response);

    TEST_ASSERT_EQUAL_MESSAGE(test_cases[i].expected_status, result.status,
                              test_cases[i].description);

    tst_edhoc_assert_error_not_empty_if_present(result.response);
    ensure_context_is_freed_on_failure(result);
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
void test_handler_fails_on_library_errors(void) {
  const struct {
    const char* description;
    void (*setup_scenario)(void);
    enum srv_edhoc_message_1_responder_status expected_status;
  } cases[] = {
      {"setup fails", tst_srv_edhoc_m1_set_process_failure,
       SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_1_PROCESS_FAILED},
      {"composition fails", tst_srv_edhoc_m2_set_compose_failure,
       SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_2_COMPOSE_FAILED},
  };

  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
    reset_mocks();
    cases[i].setup_scenario();

    const struct srv_edhoc_message_1_responder_result result =
        srv_edhoc_respond_to_message_1(env.valid_request, &env.response);

    TEST_ASSERT_EQUAL_MESSAGE(cases[i].expected_status, result.status,
                              cases[i].description);
    tst_edhoc_assert_encoded_error_is_not_empty(result.response);
    ensure_context_is_freed_on_failure(result);
  }
}