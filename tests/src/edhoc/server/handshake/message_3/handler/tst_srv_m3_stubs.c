/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Stubs for EDHOC server message 3 handler tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppParameterMayBeConst
#include "edhoc/server/handshake/message_3/handler/tst_srv_m3_stubs.h"

#include <edhoc.h>
#include <string.h>

#include "edhoc/server/handshake/common/handle_libedhoc_errors.h"
#include "unity.h"

int tst_edh_srv_message_3_stub_edhoc_process_result = EDHOC_SUCCESS;

int edhoc_message_3_process(struct edhoc_context* edhoc_context,
                            const uint8_t* message_3,
                            const size_t message_3_length) {
  (void)edhoc_context;
  (void)message_3;
  (void)message_3_length;
  return tst_edh_srv_message_3_stub_edhoc_process_result;
}

const uint8_t TST_EDH_SRV_MESSAGE_4_MOCK_PAYLOAD[] = {0x01, 0x02, 0x03, 0x04};
const size_t TST_EDH_SRV_MESSAGE_4_MOCK_LEN =
    sizeof(TST_EDH_SRV_MESSAGE_4_MOCK_PAYLOAD);

static const uint8_t* stub_message_4_compose_payload =
    TST_EDH_SRV_MESSAGE_4_MOCK_PAYLOAD;
size_t stub_message_4_compose_written_length = TST_EDH_SRV_MESSAGE_4_MOCK_LEN;

int tst_edh_srv_message_4_stub_edhoc_compose_result = EDHOC_SUCCESS;

int edhoc_message_4_compose(
    struct edhoc_context* edhoc_context,
    uint8_t* message_4,           // NOLINT(*-non-const-parameter)
    const size_t message_4_size,  // NOLINT(*-non-const-parameter)
    size_t* message_4_length) {   // NOLINT(*-non-const-parameter)
  (void)edhoc_context;
  (void)message_4_size;
  TEST_ASSERT_NOT_NULL_MESSAGE(
      message_4, "Mock Error: Destination buffer (message_4) is NULL");
  TEST_ASSERT_NOT_NULL_MESSAGE(message_4_length,
                               "Mock Error: Output length pointer is NULL");
  if (tst_edh_srv_message_4_stub_edhoc_compose_result != EDHOC_SUCCESS) {
    return tst_edh_srv_message_4_stub_edhoc_compose_result;
  }
  TEST_ASSERT_NOT_NULL_MESSAGE(stub_message_4_compose_payload,
                               "Mock Error: Internal payload source is NULL");

  if (message_4_size < stub_message_4_compose_written_length) {
    return EDHOC_ERROR_BUFFER_TOO_SMALL;
  }
  memcpy(message_4, stub_message_4_compose_payload,
         stub_message_4_compose_written_length);
  *message_4_length = stub_message_4_compose_written_length;
  return tst_edh_srv_message_4_stub_edhoc_compose_result;
}

const uint8_t TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_BUFFER[] = {0xDE, 0xAD, 0xBE,
                                                           0xEF};
const size_t TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_LEN =
    sizeof(TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_BUFFER);

void edh_srv_message_3_handler_add_error(
    const int edhoc_api_result, const char* generic_error_message,
    struct com_writable_buffer* response_data) {
  (void)edhoc_api_result;
  (void)generic_error_message;
  TEST_ASSERT_NOT_NULL_MESSAGE(response_data,
                               "Mock Error: Response buffer pointer is NULL");
  TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(response_data->capacity,
                                    TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_LEN,
                                    "Mock Error: Response buffer capacity is "
                                    "smaller than error payload length");
  memcpy(response_data->bytes, TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_BUFFER,
         TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_LEN);
  response_data->length = TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_LEN;
}

void assert_handler_writes_error_payload(struct com_writable_buffer response) {
  TEST_ASSERT_EQUAL(TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_LEN, response.length);
  TEST_ASSERT_EQUAL_MEMORY(TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_BUFFER,
                           response.bytes,
                           TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_LEN);
}

void tst_edh_srv_message_3_reset_stub_results(void) {
  tst_edh_srv_message_3_stub_edhoc_process_result = EDHOC_SUCCESS;
  tst_edh_srv_message_4_stub_edhoc_compose_result = EDHOC_SUCCESS;
  stub_message_4_compose_payload = TST_EDH_SRV_MESSAGE_4_MOCK_PAYLOAD;
  stub_message_4_compose_written_length =
      sizeof(TST_EDH_SRV_MESSAGE_4_MOCK_PAYLOAD);
}
