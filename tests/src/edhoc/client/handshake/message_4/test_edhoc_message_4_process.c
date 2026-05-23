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

static const uint8_t VALID_M4_PAYLOAD[] = {0x41, 0x42, 0x43};

static struct {
  struct edhoc_context context;
  struct com_readonly_buffer message_4;
} env = {.message_4 = {.bytes = VALID_M4_PAYLOAD,
                       .length = sizeof(VALID_M4_PAYLOAD)}};

void setUp(void) {
  tst_cli_edhoc_m4_process_reset_mock();
  env.context = (struct edhoc_context){0};
}

void test_process_message_4_ok(void) {
  const struct cli_edhoc_message_4_process_result result =
      cli_edhoc_process_message_4(&env.context, env.message_4);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG4_PROCESS_OK, result.status);
}

void test_process_message_4_fails_on_null_context(void) {
  const struct cli_edhoc_message_4_process_result result =
      cli_edhoc_process_message_4(NULL, env.message_4);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG4_PROCESS_ERR_NULL_CONTEXT, result.status);
}

void test_process_message_4_fails_on_empty_message(void) {
  const struct com_readonly_buffer empty_message = {0};

  const struct cli_edhoc_message_4_process_result result =
      cli_edhoc_process_message_4(&env.context, empty_message);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG4_PROCESS_ERR_EMPTY_MESSAGE_4, result.status);
}

void test_process_message_4_fails_when_edhoc_process_fails(void) {
  tst_cli_edhoc_m4_process_set_failure();

  const struct cli_edhoc_message_4_process_result result =
      cli_edhoc_process_message_4(&env.context, env.message_4);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG4_PROCESS_ERR_EDHOC_MESSAGE_4_PROCESS_FAILED,
                    result.status);
}
