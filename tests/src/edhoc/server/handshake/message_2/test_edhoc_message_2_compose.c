
/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 16/04/2026
 * @brief Tests for Message 2 composing
 * @see [RFC
 * 9528 5.3.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-composition-of-me)
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
#include "edhoc/server/handshake/message_2/srv_m2_compose.h"
#include "edhoc/server/handshake/mocks/message_2/tst_srv_mock_edhoc_message_2_compose.h"

enum { TST_SRV_EDHOC_COMPOSE_BUF_LEN = 256 };

static struct {
  uint8_t response_buffer[TST_SRV_EDHOC_COMPOSE_BUF_LEN];
  struct edhoc_context context;
  struct com_writable_buffer response;
} env = {.response = {.capacity = TST_SRV_EDHOC_COMPOSE_BUF_LEN}};

void setUp(void) {
  tst_srv_edhoc_m2_reset_compose_mock();
  memset(env.response_buffer, 0, sizeof(env.response_buffer));
  env.response.bytes = env.response_buffer;
  env.context = (struct edhoc_context){0};
}

void test_compose_ok_on_valid_data(void) {
  tst_srv_edhoc_m2_set_compose_ok();

  const struct srv_edhoc_message_2_compose_result result =
      srv_edhoc_compose_message_2(&env.context, env.response);

  TEST_ASSERT_EQUAL_MESSAGE(SRV_EDHOC_MSG2_COMPOSE_OK, result.status,
                            "Expected composition to succeed");
  tst_srv_edhoc_m2_compose_assert_writes_message_in_buffer(result.buffer);
}

void test_compose_fails_on_invalid_data(void) {
  const struct com_writable_buffer empty_response = {0};

  const struct {
    const char* description;
    struct edhoc_context* context;
    struct com_writable_buffer response;
    enum srv_edhoc_message_2_compose_status expected_status;
  } test_cases[] = {
      {"NULL context", NULL, env.response,
       SRV_EDHOC_MSG2_COMPOSE_ERR_NULL_CONTEXT},
      {"response buffer is empty/invalid", &env.context, empty_response,
       SRV_EDHOC_MSG2_COMPOSE_ERR_INVALID_COMPOSE_BUFFER}};

  const size_t test_case_size = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < test_case_size; i++) {
    const struct srv_edhoc_message_2_compose_result result =
        srv_edhoc_compose_message_2(test_cases[i].context,
                                    test_cases[i].response);

    TEST_ASSERT_EQUAL_MESSAGE(test_cases[i].expected_status, result.status,
                              test_cases[i].description);

    tst_edhoc_assert_error_not_empty_if_present(result.buffer);
    tst_srv_edhoc_m2_reset_compose_mock();
  }
}

void test_compose_fails_on_library_compose_failure(void) {
  tst_srv_edhoc_m2_reset_compose_mock();

  const struct srv_edhoc_message_2_compose_result result =
      srv_edhoc_compose_message_2(&env.context, env.response);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG2_COMPOSE_ERR_COMPOSE, result.status);
  tst_edhoc_assert_encoded_error_matches(
      result.buffer, "Message 2 Compose error: Composition failed",
      EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}

void test_compose_fails_when_composition_produces_empty_buffer(void) {
  tst_srv_edhoc_m2_compose_set_compose_empty_length();

  const struct srv_edhoc_message_2_compose_result result =
      srv_edhoc_compose_message_2(&env.context, env.response);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG2_COMPOSE_ERR_EMPTY_COMPOSE, result.status);
  tst_edhoc_assert_encoded_error_matches(
      result.buffer, "Message 2 Compose error: Empty compose result",
      EDHOC_ERROR_CODE_UNSPECIFIED_ERROR);
}