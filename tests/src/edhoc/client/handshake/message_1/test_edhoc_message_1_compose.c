/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 23/04/2026
 * @brief Tests for client EDHOC Message 1 compose.
 */

#include <string.h>
#include <unity.h>

#include "edhoc/client/handshake/message_1/cli_m1_compose.h"
#include "edhoc/client/handshake/mocks/message_1/tst_cli_mock_edhoc_message_1_compose.h"
#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"

enum { TST_CLI_M1_COMPOSE_BUF_LEN = 128 };

static struct {
  uint8_t compose_raw[TST_CLI_M1_COMPOSE_BUF_LEN];
  struct edhoc_context context;
  struct com_writable_buffer compose;
} env = {.compose = {.capacity = TST_CLI_M1_COMPOSE_BUF_LEN}};

void setUp(void) {
  tst_cli_edhoc_m1_compose_reset_mock();
  memset(env.compose_raw, 0, sizeof(env.compose_raw));
  env.context = (struct edhoc_context){0};
  env.compose.bytes = env.compose_raw;
  env.compose.length = 0;
}

void test_compose_message_1_ok(void) {
  tst_cli_edhoc_m1_compose_set_compose_ok();

  const struct cli_edhoc_message_1_compose_result result =
      cli_edhoc_compose_message_1(&env.context, &env.compose);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG1_COMPOSE_OK, result.status);
  tst_cli_edhoc_m1_compose_assert_writes_message_in_buffer(result.buffer);
}

void test_compose_message_1_fails_on_invalid_compose_buffer(void) {
  struct com_writable_buffer invalid_buffer = {0};

  const struct cli_edhoc_message_1_compose_result result =
      cli_edhoc_compose_message_1(&env.context, &invalid_buffer);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG1_COMPOSE_ERR_INVALID_COMPOSE_BUFFER,
                    result.status);
}

void test_compose_message_1_fails_when_prepend_flow_fails(void) {
  tst_cli_edhoc_m1_compose_set_prepend_flow_failure();

  const struct cli_edhoc_message_1_compose_result result =
      cli_edhoc_compose_message_1(&env.context, &env.compose);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG1_COMPOSE_ERR_EDHOC_PREPEND, result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.buffer);
}

void test_compose_message_1_fails_when_edhoc_compose_fails(void) {
  tst_cli_edhoc_m1_compose_set_compose_failure();

  const struct cli_edhoc_message_1_compose_result result =
      cli_edhoc_compose_message_1(&env.context, &env.compose);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG1_COMPOSE_ERR_EDHOC_MESSAGE_1_COMPOSE,
                    result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.buffer);
}

void test_compose_message_1_fails_when_size_recalculation_fails(void) {
  tst_cli_edhoc_m1_compose_set_recalculate_size_failure();

  const struct cli_edhoc_message_1_compose_result result =
      cli_edhoc_compose_message_1(&env.context, &env.compose);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG1_COMPOSE_ERR_EDHOC_PREPEND_SIZE_CALC,
                    result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.buffer);
}

void test_compose_message_1_fails_when_composition_produces_empty_buffer(void) {
  tst_cli_edhoc_m1_compose_set_compose_empty_length();

  const struct cli_edhoc_message_1_compose_result result =
      cli_edhoc_compose_message_1(&env.context, &env.compose);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG1_COMPOSE_ERR_EMPTY_COMPOSE, result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.buffer);
}
