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

#include "edhoc/server/handle_libedhoc_errors.h"

/**
 * @brief Global variables to control the behavior of the stubs in tests. Tests
 * can set these variables to simulate different scenarios and verify how the
 * code under test responds to various EDHOC library outcomes.
 */
const int SUCCESS_STUB_RESULT = EDHOC_SUCCESS;

int stub_edhoc_setup_res = SUCCESS_STUB_RESULT;
int stub_edhoc_process_res = SUCCESS_STUB_RESULT;
int stub_edhoc_compose_res = SUCCESS_STUB_RESULT;

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

const uint8_t MOCK_ERROR_PAYLOAD[] = {0xDE, 0xAD, 0xBE, 0xEF};
const size_t MOCK_ERROR_LEN = sizeof(MOCK_ERROR_PAYLOAD);

void server_edhoc_add_edhoc_error_to_response(
    const int edhoc_api_result, const struct edhoc_context* edhoc_ctx,
    common_response_buffer_t* response_data) {
  (void)edhoc_api_result;
  (void)edhoc_ctx;
  (void)response_data;
  if (response_data->payload &&
      response_data->payload_capacity >= MOCK_ERROR_LEN) {
    memcpy(response_data->payload, MOCK_ERROR_PAYLOAD, MOCK_ERROR_LEN);
    response_data->payload_length = MOCK_ERROR_LEN;
  }
}

void reset_stub_results(void) {
  stub_edhoc_setup_res = SUCCESS_STUB_RESULT;
  stub_edhoc_process_res = SUCCESS_STUB_RESULT;
  stub_edhoc_compose_res = SUCCESS_STUB_RESULT;
}