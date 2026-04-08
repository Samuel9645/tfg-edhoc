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
#include "edhoc/server/handshake/message_1/handler/tst_srv_mock_m1_hdl_deps.h"

#include <edhoc.h>
#include <string.h>

#include "common/tst_report_mock_error.h"
#include "edhoc/common/tst_mock_add_edhoc_error_info.h"
#include "edhoc/common/tst_response_buffer_helpers.h"
#include "unity.h"

// TODO: this is duplicated with m3_stubs

static const uint8_t TST_DEFAULT_M2_PAYLOAD[] = {0x01, 0x02, 0x03, 0x04};
static int setup_result = EDHOC_SUCCESS;
static int process_result = EDHOC_SUCCESS;
static int compose_result = EDHOC_SUCCESS;

static const uint8_t* compose_buffer = TST_DEFAULT_M2_PAYLOAD;
static size_t compose_written_length = sizeof(TST_DEFAULT_M2_PAYLOAD);

void tst_edh_srv_m1_configure_behavior(int setup_res, int process_res,
                                       int compose_res) {
  setup_result = setup_res;
  process_result = process_res;
  compose_result = compose_res;
}

void tst_edh_srv_m1_set_compose_length(size_t length) {
  compose_written_length = length;
}

void tst_edh_srv_hnd_reset_stub_results(void) {
  setup_result = EDHOC_SUCCESS;
  process_result = EDHOC_SUCCESS;
  compose_result = EDHOC_SUCCESS;
  compose_written_length = sizeof(TST_DEFAULT_M2_PAYLOAD);
}

void tst_edh_srv_m1_assert_handler_writes_message_2_in_buffer(
    struct com_writable_buffer response) {
  TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(compose_buffer, response.bytes,
                                       compose_written_length,
                                       "Message 2 was not written");
  TEST_ASSERT_EQUAL(compose_written_length, response.length);
}

void edh_srv_message_1_handler_add_error(
    const struct edhoc_context* context, const char* generic_error_message,
    struct com_writable_buffer* response_data) {
  (void)context;
  if (generic_error_message == NULL) {
    tst_report_mock_error("generic error message is NULL");
  }
  if (context == NULL) {
    tst_report_mock_error("context is NULL");
  }
  if (edh_com_response_buffer_is_null(response_data)) {
    return;
  }
  write_mock_buffer_to_response(response_data);
}

int edh_com_setup_context(struct edhoc_context* context,
                          const struct edhoc_credentials* credentials) {
  (void)context;
  (void)credentials;
  return setup_result;
}

int edhoc_message_1_process(struct edhoc_context* edhoc_context,
                            const uint8_t* message_1,
                            const size_t message_1_length) {
  (void)edhoc_context;
  (void)message_1;
  (void)message_1_length;
  return process_result;
}

int edhoc_message_2_compose(struct edhoc_context* edhoc_context,
                            uint8_t* message_2, const size_t message_2_size,
                            size_t* message_2_length) {
  (void)edhoc_context;
  if (message_2 == NULL) {
    tst_report_mock_error("message 2 is NULL");
    return EDHOC_ERROR_GENERIC_ERROR;
  }
  if (message_2_length == NULL) {
    tst_report_mock_error("message 2 length is NULL");
    return EDHOC_ERROR_GENERIC_ERROR;
  }
  if (compose_result != EDHOC_SUCCESS) {
    return compose_result;
  }
  if (message_2_size < compose_written_length) {
    return EDHOC_ERROR_BUFFER_TOO_SMALL;
  }
  memcpy(message_2, compose_buffer, compose_written_length);
  *message_2_length = compose_written_length;
  return compose_result;
}