/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Stubs for EDHOC server message 3 handler tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppParameterMayBeConst
#include "edhoc/server/handshake/message_3/handler/tst_srv_mock_m3_handler_deps.h"

#include <edhoc.h>
#include <string.h>

#include "../../../../../../../include/edhoc/common/add_error/com_edhoc_add_protocol_error.h"
#include "common/tst_report_mock_error.h"
#include "unity.h"

static const uint8_t TST_DEFAULT_M4_PAYLOAD[] = {0x04, 0x05, 0x06, 0x07};

static int message_3_process_result = EDHOC_SUCCESS;
static int message_4_compose_result = EDHOC_SUCCESS;
static size_t message_4_written_length = sizeof(TST_DEFAULT_M4_PAYLOAD);

void tst_srv_edhoc_m1_set_message_3_process_failure(void) {
  message_3_process_result = EDHOC_ERROR_GENERIC_ERROR;
}

void tst_srv_edhoc_m1_set_message_4_compose_failure(void) {
  message_4_compose_result = EDHOC_ERROR_GENERIC_ERROR;
}

void tst_srv_edhoc_m3_set_message_4_compose_empty_length(void) {
  message_4_written_length = 0;
}

void tst_srv_edhoc_m3_reset_stub_results(void) {
  message_3_process_result = EDHOC_SUCCESS;
  message_4_compose_result = EDHOC_SUCCESS;
  message_4_written_length = sizeof(TST_DEFAULT_M4_PAYLOAD);
}

void tst_srv_edhoc_m3_assert_handler_writes_message_4_in_buffer(
    struct com_writable_buffer response) {
  TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(TST_DEFAULT_M4_PAYLOAD, response.bytes,
                                       message_4_written_length,
                                       "message 4 content mismatch");

  TEST_ASSERT_EQUAL_MESSAGE(message_4_written_length, response.length,
                            "message 4 reported length mismatch");
}

int edhoc_message_3_process(struct edhoc_context* edhoc_context,
                            const uint8_t* message_3,
                            const size_t message_3_length) {
  (void)edhoc_context;
  (void)message_3;
  (void)message_3_length;
  return message_3_process_result;
}

int edhoc_message_4_compose(struct edhoc_context* edhoc_context,
                            uint8_t* message_4, const size_t message_4_size,
                            size_t* message_4_length) {
  (void)edhoc_context;
  if (message_4 == NULL) {
    tst_report_mock_error("message 4 destination buffer is NULL");
    return EDHOC_ERROR_GENERIC_ERROR;
  }
  if (message_4_length == NULL) {
    tst_report_mock_error("message 4 output length pointer is NULL");
    return EDHOC_ERROR_GENERIC_ERROR;
  }

  if (message_4_compose_result != EDHOC_SUCCESS) {
    return message_4_compose_result;
  }
  if (message_4_size < message_4_written_length) {
    return EDHOC_ERROR_BUFFER_TOO_SMALL;
  }
  if (message_4_written_length > 0) {
    memcpy(message_4, TST_DEFAULT_M4_PAYLOAD, message_4_written_length);
  }
  *message_4_length = message_4_written_length;
  return message_4_compose_result;
}
