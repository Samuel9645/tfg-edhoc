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

#include "edhoc/common/com_edhoc_manage_context.h"
#include "edhoc/common/tst_edhoc_params.h"
#include "edhoc/server/handshake/message_1/srv_m1_process.h"
#include "edhoc/server/handshake/message_1/srv_m1_responder.h"
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

static struct edhoc_context* create_valid_context(void) {
  struct edhoc_context* context = calloc(1, sizeof(struct edhoc_context));
  TEST_ASSERT_NOT_NULL_MESSAGE(context, "failed to allocate edhoc context");
  const struct com_edhoc_setup_context_result setup_result =
      com_edhoc_setup_context(context, tst_edhoc_get_method_0_suite_0_params());
  TEST_ASSERT_EQUAL_MESSAGE(COM_EDHOC_SETUP_CTX_OK, setup_result.status,
                            "failed to setup edhoc context");
  return context;
}

static void cleanup_context(struct edhoc_context* context) {
  if (context == NULL) {
    return;
  }
  TEST_ASSERT_EQUAL_MESSAGE(SRV_EDHOC_CLEANUP_OK,
                            srv_edhoc_cleanup_context(context),
                            "edhoc context cleanup failed");
}

void setUp(void) {
  reset_mocks();
  memset(env.response_buffer, 0, sizeof(env.response_buffer));
  env.response.bytes = env.response_buffer;
}

static struct com_readonly_buffer create_valid_request(void) {
  return (struct com_readonly_buffer){.bytes = DUMMY_REQUEST_BUFFER,
                                      .length = TST_SRV_EDHOC_HND_BUF_LEN};
}

void test_responder_ok_for_valid_data(void) {
  tst_srv_edhoc_m1_set_process_ok();
  tst_srv_edhoc_m2_set_compose_ok();
  struct edhoc_context* context = create_valid_context();

  const struct srv_edhoc_message_1_responder_result result =
      srv_edhoc_respond_to_message_1(create_valid_request(), context,
                                     env.response);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG1_RESPONDER_OK, result.status);
  tst_srv_edhoc_m2_compose_assert_writes_message_in_buffer(result.response);
  cleanup_context(context);
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
  const struct com_readonly_buffer valid_request = create_valid_request();
  const struct com_readonly_buffer empty_payload_request = {.bytes = NULL,
                                                            .length = 0};

  const struct {
    const char* description;
    struct com_readonly_buffer request;
    struct edhoc_context* context;
    struct com_writable_buffer response;
  } test_cases[] = {
      {"response buffer is empty/invalid", valid_request,
       create_valid_context(), empty_response},
      {"raw payload is empty", empty_payload_request, create_valid_context(),
       env.response},
      {"null context", valid_request, NULL, env.response},
  };

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    reset_mocks();
    const struct srv_edhoc_message_1_responder_result result =
        srv_edhoc_respond_to_message_1(test_cases[i].request,
                                       test_cases[i].context,
                                       test_cases[i].response);

    TEST_ASSERT_NOT_EQUAL_MESSAGE(SRV_EDHOC_MSG1_RESPONDER_OK, result.status,
                                  test_cases[i].description);
    cleanup_context(test_cases[i].context);
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
    struct edhoc_context* context = create_valid_context();

    const struct srv_edhoc_message_1_responder_result result =
        srv_edhoc_respond_to_message_1(create_valid_request(), context,
                                       env.response);

    TEST_ASSERT_EQUAL_MESSAGE(cases[i].expected_status, result.status,
                              cases[i].description);
    cleanup_context(context);
  }
}