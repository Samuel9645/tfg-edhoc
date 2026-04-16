/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Mocks to simulate libedhoc failures in the Message 2 composition
 * module
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "edhoc/server/handshake/message_2/tst_srv_mock_m2_compose_deps.h"

#include <edhoc.h>
#include <string.h>
#include <unity.h>

#include "common/tst_report_mock_error.h"

static int message_2_compose_result = EDHOC_SUCCESS;
static const uint8_t TST_DEFAULT_M2_PAYLOAD[] = {0x01, 0x02, 0x03, 0x04};
static const uint8_t* compose_buffer = TST_DEFAULT_M2_PAYLOAD;
static size_t compose_written_length = sizeof(TST_DEFAULT_M2_PAYLOAD);

void tst_srv_edhoc_m2_compose_set_compose_failure(void) {
  message_2_compose_result = EDHOC_ERROR_GENERIC_ERROR;
}

void tst_srv_edhoc_m2_compose_set_compose_empty_length(void) {
  message_2_compose_result = EDHOC_SUCCESS;
  compose_written_length = 0;
}

void tst_srv_edhoc_m2_compose_reset_mock_results(void) {
  message_2_compose_result = EDHOC_SUCCESS;
  compose_written_length = sizeof(TST_DEFAULT_M2_PAYLOAD);
}

void tst_srv_edhoc_m2_compose_assert_writes_message_in_buffer(
    const struct com_writable_buffer response) {
  TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(compose_buffer, response.bytes,
                                       compose_written_length,
                                       "message 2 content mismatch");
  TEST_ASSERT_EQUAL_MESSAGE(compose_written_length, response.length,
                            "message 2 reported length mismatch");
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

  if (message_2_compose_result != EDHOC_SUCCESS) {
    return message_2_compose_result;
  }
  if (message_2_size < compose_written_length) {
    return EDHOC_ERROR_BUFFER_TOO_SMALL;
  }
  memcpy(message_2, compose_buffer, compose_written_length);
  *message_2_length = compose_written_length;
  return message_2_compose_result;
}