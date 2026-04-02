/**
 * @file stubs.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Stubs to link to when testing
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
// ReSharper disable CppParameterMayBeConstPtrOrRef
// ReSharper disable CppParameterMayBeConst
// since the real implementations expects non-const pointers to allow output
// parameters
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

int tst_edh_srv_hnd_stub_edhoc_setup_res = SUCCESS_STUB_RESULT;
int tst_edh_srv_hnd_stub_edhoc_process_res = SUCCESS_STUB_RESULT;
int tst_edh_srv_hnd_stub_edhoc_compose_res = SUCCESS_STUB_RESULT;

static uint8_t stub_error_payload[200] = {0};
static size_t stub_error_len = 0;

const uint8_t TST_EDH_SRV_HND_MOCK_ERROR_PAYLOAD[] = {0xDE, 0xAD, 0xBE, 0xEF};
const size_t TST_EDH_SRV_HND_MOCK_ERROR_LEN =
    sizeof(TST_EDH_SRV_HND_MOCK_ERROR_PAYLOAD);

int edh_com_setup_context(struct edhoc_context* context,
                          const struct edhoc_credentials* credentials) {
  (void)context;
  (void)credentials;
  return tst_edh_srv_hnd_stub_edhoc_setup_res;
}

int edhoc_message_1_process(struct edhoc_context* edhoc_context,
                            const uint8_t* message_1,
                            const size_t message_1_length) {
  (void)edhoc_context;
  (void)message_1;
  (void)message_1_length;
  return tst_edh_srv_hnd_stub_edhoc_process_res;
}

int edhoc_message_2_compose(
    struct edhoc_context* edhoc_context,
    uint8_t* message_2,  // NOLINT(*-non-const-parameter)
    const size_t message_2_size,
    size_t* message_2_length) {  // NOLINT(*-non-const-parameter)
  (void)edhoc_context;
  (void)message_2;
  (void)message_2_size;
  (void)message_2_length;
  return tst_edh_srv_hnd_stub_edhoc_compose_res;
}

void tst_edh_srv_hnd_set_stub_error_response(const uint8_t* data, size_t len) {
  if (len <= sizeof(stub_error_payload)) {
    memcpy(stub_error_payload, data, len);
    stub_error_len = len;
  }
}

void edh_message_1_handler_add_error(const int edhoc_api_result, const struct edhoc_context* edhoc_context,
    const char* generic_error_message, com_response_buffer_t* response_data) {
  (void)edhoc_api_result;
  (void)edhoc_context;
  (void)generic_error_message;

  if (!response_data || !response_data->payload)
    return;

  const size_t len =
      stub_error_len > 0 ? stub_error_len : TST_EDH_SRV_HND_MOCK_ERROR_LEN;
  const uint8_t* src = stub_error_len > 0 ? stub_error_payload
                                          : TST_EDH_SRV_HND_MOCK_ERROR_PAYLOAD;

  if (response_data->payload_capacity >= len) {
    memcpy(response_data->payload, src, len);
    response_data->payload_length = len;
  }
}

void tst_edh_srv_hnd_reset_stub_results(void) {
  tst_edh_srv_hnd_stub_edhoc_setup_res = SUCCESS_STUB_RESULT;
  tst_edh_srv_hnd_stub_edhoc_process_res = SUCCESS_STUB_RESULT;
  tst_edh_srv_hnd_stub_edhoc_compose_res = SUCCESS_STUB_RESULT;
  stub_error_len = 0;
  memset(stub_error_payload, 0, sizeof(stub_error_payload));
}