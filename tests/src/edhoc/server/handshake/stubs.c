/**
 * @file libedhoc_stubs.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Stubs to link to when testing
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/stubs.h"

#include <edhoc.h>
#include <string.h>

#include "edhoc/server/handshake/common/handle_libedhoc_errors.h"
/**
 * @brief Global variables to control the behavior of the stubs in tests. Tests
 * can set these variables to simulate different scenarios and verify how the
 * code under test responds to various EDHOC library outcomes.
 */
const int SUCCESS_STUB_RESULT = EDHOC_SUCCESS;

int stub_edhoc_setup_res = SUCCESS_STUB_RESULT;
int stub_edhoc_process_res = SUCCESS_STUB_RESULT;
int stub_edhoc_compose_res = SUCCESS_STUB_RESULT;

static uint8_t stub_error_payload[200] = {0};
static size_t stub_error_len = 0;

const uint8_t MOCK_ERROR_PAYLOAD[] = {0xDE, 0xAD, 0xBE, 0xEF};
const size_t MOCK_ERROR_LEN = sizeof(MOCK_ERROR_PAYLOAD);

int edhoc_common_setup_context(struct edhoc_context* context,
                               const struct edhoc_credentials* credentials) {
  (void)context;
  (void)credentials;
  return stub_edhoc_setup_res;
}

int edhoc_message_1_process(struct edhoc_context* edhoc_context,
                            const uint8_t* message, const size_t length) {
  (void)edhoc_context;
  (void)message;
  (void)length;
  return stub_edhoc_process_res;
}

int edhoc_message_2_compose(struct edhoc_context* edhoc_context,
                            uint8_t* message_2, const size_t message_2_size,
                            size_t* message_2_length) {
  (void)edhoc_context;
  (void)message_2;
  (void)message_2_size;
  (void)message_2_length;
  return stub_edhoc_compose_res;
}

void set_stub_error_response(const uint8_t* data, size_t len) {
  if (len <= sizeof(stub_error_payload)) {
    memcpy(stub_error_payload, data, len);
    stub_error_len = len;
  }
}

void edhoc_handshake_add_message_1_error_to_response(
    const int edhoc_api_result, const struct edhoc_context* edhoc_context,
    const char* generic_error_message,
    common_response_buffer_t* response_data) {
  (void)edhoc_api_result;
  (void)edhoc_context;
  (void)generic_error_message;

  if (!response_data || !response_data->payload)
    return;

  size_t len = (stub_error_len > 0) ? stub_error_len : MOCK_ERROR_LEN;
  const uint8_t* src =
      (stub_error_len > 0) ? stub_error_payload : MOCK_ERROR_PAYLOAD;

  if (response_data->payload_capacity >= len) {
    memcpy(response_data->payload, src, len);
    response_data->payload_length = len;
  }
}

void reset_stub_results(void) {
  stub_edhoc_setup_res = SUCCESS_STUB_RESULT;
  stub_edhoc_process_res = SUCCESS_STUB_RESULT;
  stub_edhoc_compose_res = SUCCESS_STUB_RESULT;
  stub_error_len = 0;
  memset(stub_error_payload, 0, sizeof(stub_error_payload));
}