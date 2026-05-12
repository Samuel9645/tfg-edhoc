/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 16/04/2026
 * @brief Tests for Message 1 sending function
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

#include <unity.h>

#include "coap/client/cli_exchange.h"
#include "coap/client/internal/cli_exchange_internal.h"
#include "coap/client/mocks/tst_mock_exchange_coap_send_deps.h"

void setUp(void) { tst_cli_prepend_reset_all_mocks(); }

void test_prepends_cbor_true_to_the_payload_with_proper_content_format(void) {
  enum { CBOR_TRUE = 0xf5 };
  struct cli_coap_exchange dummy_exchange = {0};
  dummy_exchange.send_data = cli_coap_mock_send;
  uint8_t fake_message_1_buffer[] = {0x01, 0x02, 0x03};
  const struct com_readonly_buffer fake_message_1 = {
      .bytes = fake_message_1_buffer, .length = sizeof(fake_message_1_buffer)};

  cli_exchange_send_message_1(&dummy_exchange, fake_message_1);

  const struct cli_coap_exchange_request result =
      tst_mock_coap_get_last_sent_data();
  TEST_ASSERT_EQUAL(result.content_format, CONFIG_COAP_CONTENT_CID_EDHOC);
  TEST_ASSERT_EQUAL_UINT8(result.buffer.bytes[0], CBOR_TRUE);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(&result.buffer.bytes[1], fake_message_1_buffer,
                                fake_message_1.length);
}

void test_fails_on_invalid_data(void) {
  struct cli_coap_exchange dummy_exchange = {0};
  uint8_t dummy_data[] = {0x01, 0x02, 0x03};
  uint8_t too_large_data[CONFIG_COAP_MAX_PDU_SIZE];

  const struct {
    const char* name;
    struct cli_coap_exchange* exchange_ptr;
    struct com_readonly_buffer message;
    enum status_coap expected_status;
  } test_cases[] = {
      {.name = "Fails when exchange is NULL",
       .exchange_ptr = NULL,
       .message = {.bytes = dummy_data, .length = 3},
       .expected_status = STATUS_COAP_ERR},
      {.name = "Fails when buffer bytes are NULL",
       .exchange_ptr = &dummy_exchange,
       .message = {.bytes = NULL, .length = 3},
       .expected_status = STATUS_COAP_ERR},
      {.name = "Fails when buffer length is 0",
       .exchange_ptr = &dummy_exchange,
       .message = {.bytes = dummy_data, .length = 0},
       .expected_status = STATUS_COAP_ERR},
      {.name = "Fails when message length equals MAX_PDU_SIZE",
       .exchange_ptr = &dummy_exchange,
       .message = {.bytes = too_large_data, .length = CONFIG_COAP_MAX_PDU_SIZE},
       .expected_status = STATUS_COAP_ERR},
      {.name = "Fails when message length exceeds MAX_PDU_SIZE",
       .exchange_ptr = &dummy_exchange,
       .message = {.bytes = too_large_data,
                   .length = CONFIG_COAP_MAX_PDU_SIZE + 1},
       .expected_status = STATUS_COAP_ERR}};

  const size_t test_cases_length = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < test_cases_length; i++) {
    const enum status_coap status = cli_exchange_send_message_1(
        test_cases[i].exchange_ptr, test_cases[i].message);

    TEST_ASSERT_EQUAL_INT_MESSAGE(test_cases[i].expected_status, status,
                                  test_cases[i].name);
  }
}

void test_fails_when_prepend_dependencies_fail(void) {
  struct cli_coap_exchange dummy_exchange = {0};
  dummy_exchange.send_data = cli_coap_mock_send;
  uint8_t dummy_data[] = {0x01, 0x02, 0x03};
  const struct com_readonly_buffer msg = {.bytes = dummy_data,
                                          .length = sizeof(dummy_data)};

  const struct {
    const char* name;
    void (*setup_scenario)(void);
    enum status_coap expected_status;
  } test_cases[] = {
      {.name = "Fails when edhoc_prepend_flow returns error",
       .setup_scenario = tst_cli_prepend_flow_set_failure,
       .expected_status = STATUS_COAP_ERR},
      {.name = "Fails when edhoc_prepend_recalculate_size returns error",
       .setup_scenario = tst_cli_recalculate_size_set_failure,
       .expected_status = STATUS_COAP_ERR}};

  const size_t test_cases_length = sizeof(test_cases) / sizeof(test_cases[0]);
  for (size_t i = 0; i < test_cases_length; i++) {
    test_cases[i].setup_scenario();

    const enum status_coap actual =
        cli_exchange_send_message_1(&dummy_exchange, msg);

    TEST_ASSERT_EQUAL_INT_MESSAGE(test_cases[i].expected_status, actual,
                                  test_cases[i].name);
    tst_cli_prepend_reset_all_mocks();
  }
}
