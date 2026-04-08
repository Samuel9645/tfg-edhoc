/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Stubs to link to when testing
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
// ReSharper disable CppParameterMayBeConstPtrOrRef
// ReSharper disable CppParameterMayBeConst
// since the real implementations expects non-const pointers to allow output
// parameters
#include "edhoc/server/handshake/message_1/handler/tst_srv_m1_stubs.h"

#include <edhoc.h>
#include <string.h>

#include "edhoc/common/add_edhoc_error_info.h"
#include "edhoc/server/handshake/message_1/srv_m1_errors.h"
#include "unity.h"

// TODO: this is duplicated with m3_stubs

const int SUCCESS_STUB_RESULT = EDHOC_SUCCESS;

int tst_edh_srv_stub_context_setup_result = SUCCESS_STUB_RESULT;

int edh_com_setup_context(struct edhoc_context* context,
                          const struct edhoc_credentials* credentials) {
  (void)context;
  (void)credentials;
  return tst_edh_srv_stub_context_setup_result;
}

int tst_edh_srv_stub_message_1_process_result = SUCCESS_STUB_RESULT;

int edhoc_message_1_process(struct edhoc_context* edhoc_context,
                            const uint8_t* message_1,
                            const size_t message_1_length) {
  (void)edhoc_context;
  (void)message_1;
  (void)message_1_length;
  return tst_edh_srv_stub_message_1_process_result;
}

static const uint8_t TST_EDH_SRV_MESSAGE_2_MOCK_PAYLOAD[] = {0x01, 0x02, 0x03,
                                                             0x04};
static const size_t TST_EDH_SRV_MESSAGE_2_MOCK_LEN =
    sizeof(TST_EDH_SRV_MESSAGE_2_MOCK_PAYLOAD);

int tst_edh_srv_stub_message_2_compose_result = SUCCESS_STUB_RESULT;
const uint8_t* tst_edh_srv_stub_message_2_compose_output_buffer =
    TST_EDH_SRV_MESSAGE_2_MOCK_PAYLOAD;
size_t tst_edh_srv_stub_message_2_compose_written_length =
    TST_EDH_SRV_MESSAGE_2_MOCK_LEN;

int edhoc_message_2_compose(
    struct edhoc_context* edhoc_context,
    uint8_t* message_2,  // NOLINT(*-non-const-parameter)
    const size_t message_2_size,
    size_t* message_2_length) {  // NOLINT(*-non-const-parameter)
  (void)edhoc_context;
  (void)message_2;
  (void)message_2_size;
  (void)message_2_length;
  TEST_ASSERT_NOT_NULL_MESSAGE(
      message_2, "Mock Error: Destination buffer (message_2) is NULL");
  TEST_ASSERT_NOT_NULL_MESSAGE(message_2_length,
                               "Mock Error: Output length pointer is NULL");
  if (tst_edh_srv_stub_message_2_compose_result != SUCCESS_STUB_RESULT) {
    return tst_edh_srv_stub_message_2_compose_result;
  }
  TEST_ASSERT_NOT_NULL_MESSAGE(tst_edh_srv_stub_message_2_compose_output_buffer,
                               "Mock Error: Internal payload source is NULL");

  if (message_2_size < tst_edh_srv_stub_message_2_compose_written_length) {
    return EDHOC_ERROR_BUFFER_TOO_SMALL;
  }
  memcpy(message_2, tst_edh_srv_stub_message_2_compose_output_buffer,
         tst_edh_srv_stub_message_2_compose_written_length);
  *message_2_length = tst_edh_srv_stub_message_2_compose_written_length;
  return tst_edh_srv_stub_message_2_compose_result;
}

static const uint8_t TST_EDH_SRV_MESSAGE_1_MOCK_ERROR_BUFFER[] = {0xDE, 0xAD,
                                                                  0xBE, 0xEF};
static const size_t TST_EDH_SRV_MESSAGE_1_MOCK_ERROR_LEN =
    sizeof(TST_EDH_SRV_MESSAGE_1_MOCK_ERROR_BUFFER);

enum edh_srv_message_1_handler_add_error_status
edh_srv_message_1_handler_add_error(const int edhoc_api_result,
                                    const struct edhoc_context* edhoc_context,
                                    const char* generic_error_message,
                                    struct com_writable_buffer* response_data) {
  (void)edhoc_api_result;
  (void)edhoc_context;
  (void)generic_error_message;

  TEST_ASSERT_NOT_NULL_MESSAGE(response_data,
                               "Mock Error: Response buffer pointer is NULL");
  TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(response_data->capacity,
                                    TST_EDH_SRV_MESSAGE_1_MOCK_ERROR_LEN,
                                    "Mock Error: Response buffer capacity is "
                                    "smaller than error payload length");
  memcpy(response_data->bytes, TST_EDH_SRV_MESSAGE_1_MOCK_ERROR_BUFFER,
         TST_EDH_SRV_MESSAGE_1_MOCK_ERROR_LEN);
  response_data->length = TST_EDH_SRV_MESSAGE_1_MOCK_ERROR_LEN;
  return EDH_SRV_MSG1_ADD_ERROR_OK;
}

void tst_edh_srv_m1_assert_handler_writes_error_in_buffer(
    struct com_writable_buffer response) {
  TEST_ASSERT_EQUAL(TST_EDH_SRV_MESSAGE_1_MOCK_ERROR_LEN, response.length);
  TEST_ASSERT_EQUAL_MEMORY(TST_EDH_SRV_MESSAGE_1_MOCK_ERROR_BUFFER,
                           response.bytes,
                           TST_EDH_SRV_MESSAGE_1_MOCK_ERROR_LEN);
}

void tst_edh_srv_m1_assert_handler_writes_message_2_in_buffer(
    struct com_writable_buffer response) {
  TEST_ASSERT_EQUAL(tst_edh_srv_stub_message_2_compose_written_length,
                    response.length);
  TEST_ASSERT_EQUAL_MEMORY(tst_edh_srv_stub_message_2_compose_output_buffer,
                           response.bytes,
                           tst_edh_srv_stub_message_2_compose_written_length);
}

void tst_edh_srv_hnd_reset_stub_results(void) {
  tst_edh_srv_stub_context_setup_result = SUCCESS_STUB_RESULT;
  tst_edh_srv_stub_message_1_process_result = SUCCESS_STUB_RESULT;
  tst_edh_srv_stub_message_2_compose_result = SUCCESS_STUB_RESULT;
  tst_edh_srv_stub_message_2_compose_output_buffer =
      TST_EDH_SRV_MESSAGE_2_MOCK_PAYLOAD;
  tst_edh_srv_stub_message_2_compose_written_length =
      TST_EDH_SRV_MESSAGE_2_MOCK_LEN;
}