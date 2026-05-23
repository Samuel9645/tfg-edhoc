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

static const uint8_t VALID_M2_PAYLOAD[] = {0x31, 0x32, 0x33};

static struct {
  struct edhoc_context context;
  struct com_readonly_buffer message_2;
} env = {.message_2 = {.bytes = VALID_M2_PAYLOAD,
                       .length = sizeof(VALID_M2_PAYLOAD)}};

void setUp(void) {
  tst_cli_edhoc_m2_process_reset_mock();
  env.context = (struct edhoc_context){0};
}

void test_process_message_2_ok(void) {
  const struct cli_edhoc_message_2_process_result result =
      cli_edhoc_process_message_2(&env.context, env.message_2);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG2_PROCESS_OK, result.status);
}

void test_process_message_2_fails_on_null_context(void) {
  const struct cli_edhoc_message_2_process_result result =
      cli_edhoc_process_message_2(NULL, env.message_2);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG2_PROCESS_ERR_NULL_CONTEXT, result.status);
}

void test_process_message_2_fails_on_empty_message(void) {
  const struct com_readonly_buffer empty_message = {0};

  const struct cli_edhoc_message_2_process_result result =
      cli_edhoc_process_message_2(&env.context, empty_message);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG2_PROCESS_ERR_EMPTY_MESSAGE_2, result.status);
}

void test_process_message_2_fails_when_edhoc_process_fails(void) {
  tst_cli_edhoc_m2_process_set_failure();

  const struct cli_edhoc_message_2_process_result result =
      cli_edhoc_process_message_2(&env.context, env.message_2);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG2_PROCESS_ERR_EDHOC_MESSAGE_2_PROCESS_FAILED,
                    result.status);
}
