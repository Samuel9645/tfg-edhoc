/**
 * @file stubs.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Stubs for EDHOC server message 3 handler tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppParameterMayBeConst
#include "edhoc/server/handshake/message_3/handler/stubs.h"

#include <edhoc.h>
#include <string.h>

#include "edhoc/server/handshake/common/handle_libedhoc_errors.h"

const uint8_t TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_PAYLOAD[] = {0xDE, 0xAD, 0xBE,
                                                            0xEF};
const size_t TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_LEN =
    sizeof(TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_PAYLOAD);

int tst_edh_srv_message_3_stub_edhoc_process_result = EDHOC_SUCCESS;
int tst_edh_srv_message_3_stub_edhoc_compose_result = EDHOC_SUCCESS;

static uint8_t stub_error_payload[200] = {0};
static size_t stub_error_length = 0;

int edhoc_message_3_process(struct edhoc_context* edhoc_context,
                            const uint8_t* message_3,
                            const size_t message_3_length) {
  (void)edhoc_context;
  (void)message_3;
  (void)message_3_length;
  return tst_edh_srv_message_3_stub_edhoc_process_result;
}

int edhoc_message_4_compose(
    struct edhoc_context* edhoc_context,
    uint8_t* message_4,           // NOLINT(*-non-const-parameter)
    const size_t message_4_size,  // NOLINT(*-non-const-parameter)
    size_t* message_4_length) {   // NOLINT(*-non-const-parameter)
  (void)edhoc_context;
  (void)message_4;
  (void)message_4_size;
  (void)message_4_length;
  return tst_edh_srv_message_3_stub_edhoc_compose_result;
}

void tst_edh_srv_message_3_set_stub_error_response(const uint8_t* data,
                                                   size_t length) {
  if (length <= sizeof(stub_error_payload)) {
    memcpy(stub_error_payload, data, length);
    stub_error_length = length;
  }
}

void edh_srv_message_3_handler_add_error(const int edhoc_api_result,
                                     const char* generic_error_message,
                                     com_response_buffer_t* response_data) {
  (void)edhoc_api_result;
  (void)generic_error_message;

  if (!response_data || !response_data->buffer) {
    return;
  }

  const size_t length = stub_error_length > 0
                            ? stub_error_length
                            : TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_LEN;
  const uint8_t* source = stub_error_length > 0
                              ? stub_error_payload
                              : TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_PAYLOAD;

  if (response_data->capacity >= length) {
    memcpy(response_data->buffer, source, length);
    response_data->length = length;
  }
}

void tst_edh_srv_message_3_reset_stub_results(void) {
  tst_edh_srv_message_3_stub_edhoc_process_result = EDHOC_SUCCESS;
  tst_edh_srv_message_3_stub_edhoc_compose_result = EDHOC_SUCCESS;
  stub_error_length = 0;
  memset(stub_error_payload, 0, sizeof(stub_error_payload));
}
