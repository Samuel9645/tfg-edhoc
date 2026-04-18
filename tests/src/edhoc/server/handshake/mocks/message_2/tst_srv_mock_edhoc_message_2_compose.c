/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Mock of the EDHOC message 2 compose function of libedhoc for testing
 * the Message 2 compose module.
 * @see [libedhoc
 * documentation](https://kamil-kielbasa.github.io/libedhoc/index.html)
 * @see [RFC
 * 9528 5.3.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-composition-of-me
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/mocks/message_2/tst_srv_mock_edhoc_message_2_compose.h"

#include <edhoc.h>
#include <string.h>
#include <unity.h>

#include "common/tst_report_mock_error.h"

static bool use_real_message_2_compose = true;
static int message_2_compose_result = EDHOC_SUCCESS;
static const uint8_t TST_DEFAULT_M2_PAYLOAD[] = {0x01, 0x02, 0x03, 0x04};
static const uint8_t* compose_buffer = TST_DEFAULT_M2_PAYLOAD;
static size_t compose_written_length = sizeof(TST_DEFAULT_M2_PAYLOAD);

void tst_srv_edhoc_m2_set_compose_failure(void) {
  use_real_message_2_compose = false;
  message_2_compose_result = EDHOC_ERROR_GENERIC_ERROR;
}

void tst_srv_edhoc_m2_set_compose_ok(void) {
  use_real_message_2_compose = false;
  message_2_compose_result = EDHOC_SUCCESS;
}

void tst_srv_edhoc_m2_compose_set_compose_empty_length(void) {
  use_real_message_2_compose = false;
  message_2_compose_result = EDHOC_SUCCESS;
  compose_written_length = 0;
}

void tst_srv_edhoc_m2_reset_compose_mock(void) {
  use_real_message_2_compose = true;
  message_2_compose_result = EDHOC_SUCCESS;
  compose_written_length = sizeof(TST_DEFAULT_M2_PAYLOAD);
}

void tst_srv_edhoc_m2_compose_assert_writes_message_in_buffer(
    const struct com_readonly_buffer response) {
  TEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(compose_buffer, response.bytes,
                                       compose_written_length,
                                       "message 2 content mismatch");
  TEST_ASSERT_EQUAL_MESSAGE(compose_written_length, response.length,
                            "message 2 reported length mismatch");
}

static int mocked_message_2_compose(const struct edhoc_context* edhoc_context,
                                    uint8_t* message_2,
                                    const size_t message_2_size,
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

extern int __real_edhoc_message_2_compose(  // NOLINT(*-reserved-identifier)
                                            // we need this
    struct edhoc_context* edhoc_context, uint8_t* message_2,
    size_t message_2_size, size_t* message_2_length);

int __wrap_edhoc_message_2_compose(  // NOLINT(*-reserved-identifier)
                                     // we need this
    struct edhoc_context* edhoc_context, uint8_t* message_2,
    const size_t message_2_size, size_t* message_2_length) {
  if (use_real_message_2_compose) {
    return __real_edhoc_message_2_compose(edhoc_context, message_2,
                                          message_2_size, message_2_length);
  }
  return mocked_message_2_compose(edhoc_context, message_2, message_2_size,
                                  message_2_length);
}