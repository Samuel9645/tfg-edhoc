/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Stubs for EDHOC server message 3 handler tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppParameterMayBeConst
#include "edhoc/server/handshake/message_3/handler/tst_srv_mock_m3_hdl_deps.h"

#include <edhoc.h>
#include <string.h>

#include "edhoc/common/add_edhoc_error_info.h"
#include "unity.h"

const int SUCCESS_STUB_RESULT = EDHOC_SUCCESS;

int tst_edh_srv_stub_process_message_3_result = SUCCESS_STUB_RESULT;

int edhoc_message_3_process(struct edhoc_context* edhoc_context,
                            const uint8_t* message_3,
                            const size_t message_3_length) {
  (void)edhoc_context;
  (void)message_3;
  (void)message_3_length;
  return tst_edh_srv_stub_process_message_3_result;
}

static const uint8_t TST_EDH_SRV_MESSAGE_4_MOCK_PAYLOAD[] = {0x01, 0x02, 0x03,
                                                             0x04};
static const size_t TST_EDH_SRV_MESSAGE_4_MOCK_LEN =
    sizeof(TST_EDH_SRV_MESSAGE_4_MOCK_PAYLOAD);

size_t tst_edh_srv_stub_message_4_compose_written_length =
    TST_EDH_SRV_MESSAGE_4_MOCK_LEN;
const uint8_t* tst_edh_srv_stub_message_4_compose_output_buffer =
    TST_EDH_SRV_MESSAGE_4_MOCK_PAYLOAD;
int tst_edh_srv_stub_message_4_compose_result = SUCCESS_STUB_RESULT;

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
  if (tst_edh_srv_stub_message_4_compose_result != SUCCESS_STUB_RESULT) {
    return tst_edh_srv_stub_message_4_compose_result;
  }
  TEST_ASSERT_NOT_NULL_MESSAGE(tst_edh_srv_stub_message_4_compose_output_buffer,
                               "Mock Error: Internal payload source is NULL");

  if (message_4_size < tst_edh_srv_stub_message_4_compose_written_length) {
    return EDHOC_ERROR_BUFFER_TOO_SMALL;
  }
  memcpy(message_4, tst_edh_srv_stub_message_4_compose_output_buffer,
         tst_edh_srv_stub_message_4_compose_written_length);
  *message_4_length = tst_edh_srv_stub_message_4_compose_written_length;
  return tst_edh_srv_stub_message_4_compose_result;
}

void tst_edh_srv_m1_assert_handler_writes_message_4_in_buffer(
    struct com_writable_buffer response) {
  TEST_ASSERT_EQUAL_MESSAGE(tst_edh_srv_stub_message_4_compose_written_length,
                            response.length,
                            "writen length pointer is not the expected length");
  TEST_ASSERT_EQUAL_MEMORY_MESSAGE(
      tst_edh_srv_stub_message_4_compose_output_buffer, response.bytes,
      tst_edh_srv_stub_message_4_compose_written_length,
      "The composed message 4 payload was not written in the response buffer");
}

void tst_edh_srv_message_3_reset_stub_results(void) {
  tst_edh_srv_stub_process_message_3_result = SUCCESS_STUB_RESULT;
  tst_edh_srv_stub_message_4_compose_result = SUCCESS_STUB_RESULT;
  tst_edh_srv_stub_message_4_compose_output_buffer =
      TST_EDH_SRV_MESSAGE_4_MOCK_PAYLOAD;
  tst_edh_srv_stub_message_4_compose_written_length =
      TST_EDH_SRV_MESSAGE_4_MOCK_LEN;
}
