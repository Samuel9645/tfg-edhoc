/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Tests for Message 4 composing.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include <edhoc.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"
#include "edhoc/server/handshake/message_4/srv_m4_compose.h"
#include "edhoc/server/handshake/mocks/message_4/tst_srv_mock_edhoc_message_4_compose.h"

enum { TST_SRV_EDHOC_COMPOSE_BUF_LEN = 256 };

static struct {
  uint8_t response_buffer[TST_SRV_EDHOC_COMPOSE_BUF_LEN];
  struct edhoc_context context;
  struct com_writable_buffer response;
} env = {.response = {.capacity = TST_SRV_EDHOC_COMPOSE_BUF_LEN}};

void setUp(void) {
  tst_srv_edhoc_m4_reset_compose_mock();
  memset(env.response_buffer, 0, sizeof(env.response_buffer));
  env.response.bytes = env.response_buffer;
  env.response.length = 0;
  env.context = (struct edhoc_context){0};
}

void test_compose_ok_on_valid_data(void) {
  tst_srv_edhoc_m4_set_compose_ok();

  const struct srv_edhoc_message_4_compose_result result =
      srv_edhoc_compose_message_4(&env.context, &env.response);

  TEST_ASSERT_EQUAL_MESSAGE(SRV_EDHOC_MSG4_COMPOSE_OK, result.status,
                            "Expected composition to succeed");
  tst_srv_edhoc_m4_compose_assert_writes_message_in_buffer(env.response);
}

static int response_is_valid(struct com_writable_buffer* response) {
  return response != NULL && response->bytes != NULL;
}

void test_compose_fails_on_invalid_data(void) {
  struct com_writable_buffer empty_response = {0};

  const struct {
    const char* description;
    struct edhoc_context* context;
    struct com_writable_buffer* response;
    enum srv_edhoc_message_4_compose_status expected_status;
  } test_cases[] = {
      {"NULL context", NULL, &env.response,
       SRV_EDHOC_MSG4_COMPOSE_ERR_NULL_CONTEXT},
      {"response buffer is NULL", &env.context, NULL,
       SRV_EDHOC_MSG4_COMPOSE_ERR_INVALID_COMPOSE_BUFFER},
      {"response buffer is empty/invalid", &env.context, &empty_response,
       SRV_EDHOC_MSG4_COMPOSE_ERR_INVALID_COMPOSE_BUFFER}};

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    struct com_writable_buffer* response = test_cases[i].response;

    const struct srv_edhoc_message_4_compose_result result =
        srv_edhoc_compose_message_4(test_cases[i].context, response);

    TEST_ASSERT_EQUAL_MESSAGE(test_cases[i].expected_status, result.status,
                              test_cases[i].description);

    if (response_is_valid(response)) {
      tst_edhoc_assert_encoded_error_is_not_empty(*response);
    }
    tst_srv_edhoc_m4_reset_compose_mock();
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
void test_compose_fails_on_library_compose_failure(void) {
  tst_srv_edhoc_m4_reset_compose_mock();

  const struct srv_edhoc_message_4_compose_result result =
      srv_edhoc_compose_message_4(&env.context, &env.response);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG4_COMPOSE_ERR_COMPOSE_FAILED, result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(env.response);
}

void test_compose_fails_when_composition_produces_empty_buffer(void) {
  tst_srv_edhoc_m4_compose_set_compose_empty_length();

  const struct srv_edhoc_message_4_compose_result result =
      srv_edhoc_compose_message_4(&env.context, &env.response);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG4_COMPOSE_ERR_EMPTY_COMPOSE, result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(env.response);
}
