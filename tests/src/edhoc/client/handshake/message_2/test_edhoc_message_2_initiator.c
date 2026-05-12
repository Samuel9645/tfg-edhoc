/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 23/04/2026
 * @brief Tests for client EDHOC Message 2 initiator flow.
 */

#include <string.h>
#include <unity.h>

#include "edhoc/client/handshake/message_2/cli_m2_initiator.h"
#include "edhoc/client/handshake/mocks/message_2/tst_cli_mock_edhoc_message_2_process.h"
#include "edhoc/client/handshake/mocks/message_3/tst_cli_mock_edhoc_message_3_compose.h"
#include "edhoc/common/add_error/common/tst_edhoc_add_error_assertions.h"

enum { TST_CLI_M2_INITIATOR_REQ_BUF_LEN = 128 };

static const uint8_t VALID_M2_PAYLOAD[] = {0x52, 0x53, 0x54};

static struct {
  struct edhoc_context context;
  struct cli_edhoc_message_2_initiator_request request;
  uint8_t request_raw[TST_CLI_M2_INITIATOR_REQ_BUF_LEN];
  struct com_writable_buffer request_buffer;
} env = {
    .request = {.raw_payload = {.bytes = VALID_M2_PAYLOAD,
                                .length = sizeof(VALID_M2_PAYLOAD)}},
    .request_buffer = {.capacity = TST_CLI_M2_INITIATOR_REQ_BUF_LEN},
};

void setUp(void) {
  tst_cli_edhoc_m2_process_reset_mock();
  tst_cli_edhoc_m3_compose_reset_mock();
  memset(env.request_raw, 0, sizeof(env.request_raw));
  env.context = (struct edhoc_context){0};
  env.request.edhoc_context = &env.context;
  env.request_buffer.bytes = env.request_raw;
}

void test_message_2_initiator_ok(void) {
  tst_cli_edhoc_m3_compose_set_compose_ok();

  const struct cli_edhoc_message_2_initiator_result result =
      cli_edhoc_respond_to_message_2(env.request, env.request_buffer);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG2_INITIATOR_OK, result.status);
  tst_cli_edhoc_m3_compose_assert_writes_message_in_buffer(result.buffer);
}

void test_message_2_initiator_fails_on_invalid_request_buffer(void) {
  const struct com_writable_buffer invalid_request_buffer = {0};

  const struct cli_edhoc_message_2_initiator_result result =
      cli_edhoc_respond_to_message_2(env.request, invalid_request_buffer);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG2_INITIATOR_ERR_INVALID_RESPONSE_BUFFER,
                    result.status);
}

void test_message_2_initiator_fails_when_message_2_process_fails(void) {
  tst_cli_edhoc_m2_process_set_failure();

  const struct cli_edhoc_message_2_initiator_result result =
      cli_edhoc_respond_to_message_2(env.request, env.request_buffer);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG2_INITIATOR_ERR_MESSAGE_2_PROCESS,
                    result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.buffer);
}

void test_message_2_initiator_fails_when_message_3_compose_fails(void) {
  tst_cli_edhoc_m3_compose_set_compose_failure();

  const struct cli_edhoc_message_2_initiator_result result =
      cli_edhoc_respond_to_message_2(env.request, env.request_buffer);

  TEST_ASSERT_EQUAL(CLI_EDHOC_MSG2_INITIATOR_ERR_MESSAGE_3_COMPOSE,
                    result.status);
  tst_edhoc_assert_encoded_error_is_not_empty(result.buffer);
}
