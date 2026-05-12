/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 23/04/2026
 * @brief Mock dependencies for client Message 1 compose tests.
 */

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "edhoc/client/handshake/mocks/message_1/tst_cli_mock_edhoc_message_1_compose.h"

#include <edhoc.h>
#include <edhoc_helpers.h>
#include <string.h>
#include <unity.h>

#include "common/tst_report_mock_error.h"

static int message_1_compose_result = EDHOC_SUCCESS;
static bool use_real_message_1_compose = true;
static const uint8_t TST_DEFAULT_M1_PAYLOAD[] = {0x10, 0x11, 0x12, 0x13};
static const uint8_t* compose_payload = TST_DEFAULT_M1_PAYLOAD;
static size_t compose_payload_length = sizeof(TST_DEFAULT_M1_PAYLOAD);

void tst_cli_edhoc_m1_compose_set_compose_failure(void) {
  use_real_message_1_compose = false;
  message_1_compose_result = EDHOC_ERROR_NOT_PERMITTED;
}

void tst_cli_edhoc_m1_compose_set_compose_empty_length(void) {
  use_real_message_1_compose = false;
  message_1_compose_result = EDHOC_SUCCESS;
  compose_payload_length = 0;
}

void tst_cli_edhoc_m1_compose_set_compose_ok(void) {
  use_real_message_1_compose = false;
  message_1_compose_result = EDHOC_SUCCESS;
}

void tst_cli_edhoc_m1_compose_reset_mock(void) {
  use_real_message_1_compose = true;
  message_1_compose_result = EDHOC_SUCCESS;
  compose_payload = TST_DEFAULT_M1_PAYLOAD;
  compose_payload_length = sizeof(TST_DEFAULT_M1_PAYLOAD);
}

void tst_cli_edhoc_m1_compose_assert_writes_message_in_buffer(
    const struct com_readonly_buffer message_1) {
  TEST_ASSERT_EQUAL_size_t(compose_payload_length, message_1.length);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(compose_payload, message_1.bytes,
                               compose_payload_length);
}

static int mocked_message_1_compose(const struct edhoc_context* edhoc_context,
                                    uint8_t* message_1,
                                    const size_t message_1_size,
                                    size_t* message_1_length) {
  (void)edhoc_context;
  if (message_1 == NULL) {
    tst_report_mock_error("message 1 destination buffer is NULL");
    return EDHOC_ERROR_INVALID_ARGUMENT;
  }
  if (message_1_length == NULL) {
    tst_report_mock_error("message 1 output length pointer is NULL");
    return EDHOC_ERROR_INVALID_ARGUMENT;
  }

  if (message_1_compose_result != EDHOC_SUCCESS) {
    return message_1_compose_result;
  }
  if (message_1_size < compose_payload_length) {
    return EDHOC_ERROR_BUFFER_TOO_SMALL;
  }
  if (compose_payload_length > 0) {
    memcpy(message_1, compose_payload, compose_payload_length);
  }
  *message_1_length = compose_payload_length;
  return EDHOC_SUCCESS;
}

extern int __real_edhoc_message_1_compose(  // NOLINT(*-reserved-identifier)
                                            // we need this
    struct edhoc_context* edhoc_context, uint8_t* message_1,
    size_t message_1_size, size_t* message_1_length);

int __wrap_edhoc_message_1_compose(  // NOLINT(*-reserved-identifier)
                                     // we need this
    struct edhoc_context* edhoc_context, uint8_t* message_1,
    const size_t message_1_size, size_t* message_1_length) {
  if (use_real_message_1_compose) {
    return __real_edhoc_message_1_compose(edhoc_context, message_1,
                                          message_1_size, message_1_length);
  }
  return mocked_message_1_compose(edhoc_context, message_1, message_1_size,
                                  message_1_length);
}

