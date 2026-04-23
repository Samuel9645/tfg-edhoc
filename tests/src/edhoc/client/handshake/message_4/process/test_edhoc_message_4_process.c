/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 23/04/2026
 * @brief Tests for client EDHOC Message 4 process.
 */

#include <string.h>
#include <unity.h>

#include "edhoc/client/handshake/message_4/cli_m4_process.h"
#include "edhoc/client/handshake/mocks/message_4/tst_cli_mock_edhoc_message_4_process.h"
#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"

enum { TST_CLI_M4_PROCESS_ERROR_BUF_LEN = 128 };

static const uint8_t VALID_M4_PAYLOAD[] = {0x41, 0x42, 0x43};

static struct {
  struct edhoc_context context;
  struct com_readonly_buffer message_4;
  uint8_t error_raw[TST_CLI_M4_PROCESS_ERROR_BUF_LEN];
  struct com_writable_buffer error;
} env = {.message_4 = {.bytes = VALID_M4_PAYLOAD,
                       .length = sizeof(VALID_M4_PAYLOAD)},
         .error = {.capacity = TST_CLI_M4_PROCESS_ERROR_BUF_LEN}};

void setUp(void) {
  tst_cli_edhoc_m4_process_reset_mock();
  memset(env.error_raw, 0, sizeof(env.error_raw));
  env.context = (struct edhoc_context){0};
  env.error.bytes = env.error_raw;
  env.error.length = 0;
}

void test_process_message_4_ok(void) {
  const struct cli_edhoc_message_4_process_result result =
      cli_edhoc_process_message_4(&env.context, env.message_4, &env.error);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG4_PROCESS_OK, result.status);
  TEST_ASSERT_EQUAL(0, env.error.length);
}

void test_process_message_4_fails_on_invalid_error_buffer(void) {
  struct com_writable_buffer invalid_error = {0};

  const struct cli_edhoc_message_4_process_result result =
      cli_edhoc_process_message_4(&env.context, env.message_4, &invalid_error);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG4_PROCESS_ERR_INVALID_ERROR_BUFFER,
                    result.status);
}

void test_process_message_4_fails_on_null_context(void) {
  const struct cli_edhoc_message_4_process_result result =
      cli_edhoc_process_message_4(NULL, env.message_4, &env.error);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG4_PROCESS_ERR_NULL_CONTEXT, result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.error_buffer);
}

void test_process_message_4_fails_on_empty_message(void) {
  const struct com_readonly_buffer empty_message = {0};

  const struct cli_edhoc_message_4_process_result result =
      cli_edhoc_process_message_4(&env.context, empty_message, &env.error);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG4_PROCESS_ERR_EMPTY_MESSAGE_4, result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.error_buffer);
}

void test_process_message_4_fails_when_edhoc_process_fails(void) {
  tst_cli_edhoc_m4_process_set_failure();

  const struct cli_edhoc_message_4_process_result result =
      cli_edhoc_process_message_4(&env.context, env.message_4, &env.error);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG4_PROCESS_ERR_EDHOC_MESSAGE_4_PROCESS_FAILED,
                    result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.error_buffer);
}
