/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 23/04/2026
 * @brief Mock dependencies for client Message 3 compose tests.
 */

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "edhoc/client/handshake/mocks/message_3/tst_cli_mock_edhoc_message_3_compose.h"

#include <edhoc.h>
#include <edhoc_helpers.h>
#include <string.h>
#include <unity.h>

#include "common/tst_report_mock_error.h"

static bool use_real_message_3_compose = true;
static int message_3_compose_result = EDHOC_SUCCESS;
static const uint8_t TST_DEFAULT_M3_PAYLOAD[] = {0x20, 0x21, 0x22, 0x23};
static const uint8_t* compose_payload = TST_DEFAULT_M3_PAYLOAD;
static size_t compose_payload_length = sizeof(TST_DEFAULT_M3_PAYLOAD);

void tst_cli_edhoc_m3_compose_set_compose_failure(void) {
  use_real_message_3_compose = false;
  message_3_compose_result = EDHOC_ERROR_NOT_PERMITTED;
}

void tst_cli_edhoc_m3_compose_set_compose_empty_length(void) {
  use_real_message_3_compose = false;
  message_3_compose_result = EDHOC_SUCCESS;
  compose_payload_length = 0;
}

void tst_cli_edhoc_m3_compose_set_compose_ok(void) {
  use_real_message_3_compose = false;
  message_3_compose_result = EDHOC_SUCCESS;
}

void tst_cli_edhoc_m3_compose_reset_mock(void) {
  use_real_message_3_compose = true;
  message_3_compose_result = EDHOC_SUCCESS;
  compose_payload = TST_DEFAULT_M3_PAYLOAD;
  compose_payload_length = sizeof(TST_DEFAULT_M3_PAYLOAD);
}

void tst_cli_edhoc_m3_compose_assert_writes_message_in_buffer(
    const struct com_readonly_buffer message_3) {
  TEST_ASSERT_EQUAL_size_t(compose_payload_length, message_3.length);
  TEST_ASSERT_EQUAL_HEX8_ARRAY(compose_payload, message_3.bytes,
                               compose_payload_length);
}

static int mocked_message_3_compose(const struct edhoc_context* edhoc_context,
                                    uint8_t* message_3,
                                    const size_t message_3_size,
                                    size_t* message_3_length) {
  (void)edhoc_context;
  if (message_3 == NULL) {
    tst_report_mock_error("message 3 destination buffer is NULL");
    return EDHOC_ERROR_INVALID_ARGUMENT;
  }
  if (message_3_length == NULL) {
    tst_report_mock_error("message 3 output length pointer is NULL");
    return EDHOC_ERROR_INVALID_ARGUMENT;
  }

  if (message_3_compose_result != EDHOC_SUCCESS) {
    return message_3_compose_result;
  }
  if (message_3_size < compose_payload_length) {
    return EDHOC_ERROR_BUFFER_TOO_SMALL;
  }
  if (compose_payload_length > 0) {
    memcpy(message_3, compose_payload, compose_payload_length);
  }

  *message_3_length = compose_payload_length;
  return EDHOC_SUCCESS;
}

extern int __real_edhoc_message_3_compose(  // NOLINT(*-reserved-identifier)
                                            // we need this
    struct edhoc_context* edhoc_context, uint8_t* message_3,
    size_t message_3_size, size_t* message_3_length);

int __wrap_edhoc_message_3_compose(  // NOLINT(*-reserved-identifier)
                                     // we need this
    struct edhoc_context* edhoc_context, uint8_t* message_3,
    const size_t message_3_size, size_t* message_3_length) {
  if (use_real_message_3_compose) {
    return __real_edhoc_message_3_compose(edhoc_context, message_3,
                                          message_3_size, message_3_length);
  }
  return mocked_message_3_compose(edhoc_context, message_3, message_3_size,
                                  message_3_length);
}

