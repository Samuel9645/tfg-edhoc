/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 16/04/2026
 * @brief Tests for Message 3 sending function
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

#include <string.h>
#include <unity.h>

#include "coap/client/cli_exchange.h"
#include "coap/client/internal/cli_exchange_internal.h"
#include "coap/client/mocks/tst_mock_exchange_coap_send_deps.h"
#include "edhoc/common/com_edhoc_setup_context.h"

void setUp(void) { tst_cli_prepend_reset_all_mocks(); }

void test_prepends_connection_id_to_the_payload_with_proper_content_format(
    void) {
  struct cli_coap_exchange dummy_exchange = {0};
  dummy_exchange.send_data = cli_coap_mock_send;
  struct edhoc_context context = {0};
  int8_t positive_id = 15;
  struct edhoc_connection_id fake_connection_id = {
      .encode_type = EDHOC_CID_TYPE_ONE_BYTE_INTEGER, .int_value = positive_id};
  context.private_peer_cid = fake_connection_id;
  uint8_t fake_message_3_buffer[] = {0x04, 0x05, 0x06};
  const struct com_readonly_buffer fake_message_3 = {
      .bytes = fake_message_3_buffer, .length = sizeof(fake_message_3_buffer)};

  cli_exchange_send_message_3(&dummy_exchange, &context, fake_message_3);

  const struct cli_coap_exchange_request result =
      tst_mock_coap_get_last_sent_data();

  TEST_ASSERT_EQUAL(result.content_format, CONFIG_COAP_CONTENT_CID_EDHOC);
  TEST_ASSERT_EQUAL_INT8(result.buffer.bytes[0], positive_id);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(
      &result.buffer.bytes[result.buffer.length - fake_message_3.length],
      fake_message_3_buffer, fake_message_3.length);
}

void test_fails_on_invalid_data(void) {
  struct cli_coap_exchange dummy_exchange = {0};
  struct edhoc_context dummy_context = {0};
  uint8_t dummy_data[] = {0x01, 0x02, 0x03};
  uint8_t too_large_data[CONFIG_COAP_MAX_PDU_SIZE];

  const struct {
    const char* name;
    struct cli_coap_exchange* exchange_ptr;
    const struct edhoc_context* context_ptr;
    struct com_readonly_buffer message;
    enum status_coap expected_status;
  } test_cases[] = {
      {.name = "Fails when exchange is NULL",
       .exchange_ptr = NULL,
       .context_ptr = &dummy_context,
       .message = {.bytes = dummy_data, .length = 3},
       .expected_status = STATUS_COAP_ERR},
      {.name = "Fails when context is NULL",
       .exchange_ptr = &dummy_exchange,
       .context_ptr = NULL,
       .message = {.bytes = dummy_data, .length = 3},
       .expected_status = STATUS_COAP_ERR},
      {.name = "Fails when buffer bytes are NULL",
       .exchange_ptr = &dummy_exchange,
       .context_ptr = &dummy_context,
       .message = {.bytes = NULL, .length = 3},
       .expected_status = STATUS_COAP_ERR},
      {.name = "Fails when buffer length is 0",
       .exchange_ptr = &dummy_exchange,
       .context_ptr = &dummy_context,
       .message = {.bytes = dummy_data, .length = 0},
       .expected_status = STATUS_COAP_ERR},
      {.name = "Fails when message length equals MAX_PDU_SIZE",
       .exchange_ptr = &dummy_exchange,
       .context_ptr = &dummy_context,
       .message = {.bytes = too_large_data, .length = CONFIG_COAP_MAX_PDU_SIZE},
       .expected_status = STATUS_COAP_ERR},
      {.name = "Fails when message length exceeds MAX_PDU_SIZE",
       .exchange_ptr = &dummy_exchange,
       .context_ptr = &dummy_context,
       .message = {.bytes = too_large_data,
                   .length = CONFIG_COAP_MAX_PDU_SIZE + 1},
       .expected_status = STATUS_COAP_ERR}};

  const size_t test_cases_length = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < test_cases_length; i++) {
    const enum status_coap status = cli_exchange_send_message_3(
        test_cases[i].exchange_ptr, test_cases[i].context_ptr,
        test_cases[i].message);

    TEST_ASSERT_EQUAL_INT_MESSAGE(test_cases[i].expected_status, status,
                                  test_cases[i].name);
  }
}

void test_fails_when_prepend_dependencies_fail(void) {
  struct cli_coap_exchange dummy_exchange = {0};
  dummy_exchange.send_data = cli_coap_mock_send;
  struct edhoc_context dummy_context = {0};

  uint8_t dummy_data[] = {0x01, 0x02, 0x03};
  const struct com_readonly_buffer msg = {.bytes = dummy_data,
                                          .length = sizeof(dummy_data)};

  const struct {
    const char* name;
    void (*setup_scenario)(void);
    enum status_coap expected_status;
  } test_cases[] = {
      {.name = "Fails when edhoc_prepend_connection_id returns error",
       .setup_scenario = tst_cli_prepend_connection_id_set_failure,
       .expected_status = STATUS_COAP_ERR},
      {.name = "Fails when edhoc_prepend_recalculate_size returns error",
       .setup_scenario = tst_cli_recalculate_size_set_failure,
       .expected_status = STATUS_COAP_ERR}};

  const size_t test_cases_length = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < test_cases_length; i++) {
    test_cases[i].setup_scenario();

    const enum status_coap actual =
        cli_exchange_send_message_3(&dummy_exchange, &dummy_context, msg);

    TEST_ASSERT_EQUAL_INT_MESSAGE(test_cases[i].expected_status, actual,
                                  test_cases[i].name);
    tst_cli_prepend_reset_all_mocks();
  }
}