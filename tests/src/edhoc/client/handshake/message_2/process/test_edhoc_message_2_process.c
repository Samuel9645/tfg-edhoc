/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 23/04/2026
 * @brief Tests for client EDHOC Message 2 process.
 */

#include <string.h>
#include <unity.h>

#include "edhoc/client/handshake/message_2/cli_m2_process.h"
#include "edhoc/client/handshake/mocks/message_2/tst_cli_mock_edhoc_message_2_process.h"
#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"

enum { TST_CLI_M2_PROCESS_ERROR_BUF_LEN = 128 };

static const uint8_t VALID_M2_PAYLOAD[] = {0x31, 0x32, 0x33};

static struct {
  struct edhoc_context context;
  struct com_readonly_buffer message_2;
  uint8_t error_raw[TST_CLI_M2_PROCESS_ERROR_BUF_LEN];
  struct com_writable_buffer error;
} env = {.message_2 = {.bytes = VALID_M2_PAYLOAD,
                       .length = sizeof(VALID_M2_PAYLOAD)},
         .error = {.capacity = TST_CLI_M2_PROCESS_ERROR_BUF_LEN}};

void setUp(void) {
  tst_cli_edhoc_m2_process_reset_mock();
  memset(env.error_raw, 0, sizeof(env.error_raw));
  env.context = (struct edhoc_context){0};
  env.error.bytes = env.error_raw;
  env.error.length = 0;
}

void test_process_message_2_ok(void) {
  const struct cli_edhoc_message_2_process_result result =
      cli_edhoc_process_message_2(&env.context, env.message_2, &env.error);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG2_PROCESS_OK, result.status);
  TEST_ASSERT_EQUAL(0, env.error.length);
}

void test_process_message_2_fails_on_invalid_error_buffer(void) {
  struct com_writable_buffer invalid_error_buffer = {0};

  const struct cli_edhoc_message_2_process_result result =
      cli_edhoc_process_message_2(&env.context, env.message_2,
                                  &invalid_error_buffer);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG2_PROCESS_ERR_INVALID_ERROR_BUFFER,
                    result.status);
}

void test_process_message_2_fails_on_null_context(void) {
  const struct cli_edhoc_message_2_process_result result =
      cli_edhoc_process_message_2(NULL, env.message_2, &env.error);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG2_PROCESS_ERR_NULL_CONTEXT, result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.error_buffer);
}

void test_process_message_2_fails_on_empty_message(void) {
  const struct com_readonly_buffer empty_message = {0};

  const struct cli_edhoc_message_2_process_result result =
      cli_edhoc_process_message_2(&env.context, empty_message, &env.error);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG2_PROCESS_ERR_EMPTY_MESSAGE_2, result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.error_buffer);
}

void test_process_message_2_fails_when_edhoc_process_fails(void) {
  tst_cli_edhoc_m2_process_set_failure();

  const struct cli_edhoc_message_2_process_result result =
      cli_edhoc_process_message_2(&env.context, env.message_2, &env.error);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG2_PROCESS_ERR_EDHOC_MESSAGE_2_PROCESS_FAILED,
                    result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.error_buffer);
}
