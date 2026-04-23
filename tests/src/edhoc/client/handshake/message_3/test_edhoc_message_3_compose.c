/**
 * @file
 * @brief Tests for client EDHOC Message 3 compose.
 */

#include <string.h>
#include <unity.h>

#include "edhoc/client/handshake/message_3/cli_m3_compose.h"
#include "edhoc/client/handshake/mocks/message_3/tst_cli_mock_edhoc_message_3_compose.h"
#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"

enum { TST_CLI_M3_COMPOSE_BUF_LEN = 128 };

static struct {
  uint8_t compose_raw[TST_CLI_M3_COMPOSE_BUF_LEN];
  struct edhoc_context context;
  struct com_writable_buffer compose;
} env = {.compose = {.capacity = TST_CLI_M3_COMPOSE_BUF_LEN}};

void setUp(void) {
  tst_cli_edhoc_m3_compose_reset_mock();
  memset(env.compose_raw, 0, sizeof(env.compose_raw));
  env.context = (struct edhoc_context){0};
  env.compose.bytes = env.compose_raw;
  env.compose.length = 0;
}

void test_compose_message_3_ok(void) {
  tst_cli_edhoc_m3_compose_set_compose_ok();

  const struct cli_edhoc_message_3_compose_result result =
      cli_edhoc_compose_message_3(&env.context, &env.compose);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG3_COMPOSE_OK, result.status);
  tst_cli_edhoc_m3_compose_assert_writes_message_in_buffer(result.buffer);
}

void test_compose_message_3_fails_on_invalid_compose_buffer(void) {
  struct com_writable_buffer invalid_buffer = {0};

  const struct cli_edhoc_message_3_compose_result result =
      cli_edhoc_compose_message_3(&env.context, &invalid_buffer);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG3_COMPOSE_ERR_INVALID_COMPOSE_BUFFER,
                    result.status);
}

void test_compose_message_3_fails_on_null_context(void) {
  const struct cli_edhoc_message_3_compose_result result =
      cli_edhoc_compose_message_3(NULL, &env.compose);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG3_COMPOSE_ERR_NULL_CONTEXT, result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.buffer);
}

void test_compose_message_3_fails_when_prepend_connection_id_fails(void) {
  tst_cli_edhoc_m3_compose_set_prepend_connection_id_failure();

  const struct cli_edhoc_message_3_compose_result result =
      cli_edhoc_compose_message_3(&env.context, &env.compose);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG3_COMPOSE_ERR_CONNECTION_ID_PREPEND_FAILED,
                    result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.buffer);
}

void test_compose_message_3_fails_when_edhoc_compose_fails(void) {
  tst_cli_edhoc_m3_compose_set_compose_failure();

  const struct cli_edhoc_message_3_compose_result result =
      cli_edhoc_compose_message_3(&env.context, &env.compose);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG3_COMPOSE_ERR_EDHOC_MESSAGE_3_COMPOSE_FAILED,
                    result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.buffer);
}

void test_compose_message_3_fails_when_size_recalculation_fails(void) {
  tst_cli_edhoc_m3_compose_set_recalculate_size_failure();

  const struct cli_edhoc_message_3_compose_result result =
      cli_edhoc_compose_message_3(&env.context, &env.compose);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG3_COMPOSE_ERR_PREPEND_RECALCULATION_FAILED,
                    result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.buffer);
}

void test_compose_message_3_fails_when_composition_produces_empty_buffer(void) {
  tst_cli_edhoc_m3_compose_set_compose_empty_length();

  const struct cli_edhoc_message_3_compose_result result =
      cli_edhoc_compose_message_3(&env.context, &env.compose);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG3_COMPOSE_ERR_EMPTY_COMPOSE, result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.buffer);
}
