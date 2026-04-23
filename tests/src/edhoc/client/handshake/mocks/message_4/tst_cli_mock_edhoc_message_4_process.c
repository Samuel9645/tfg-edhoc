/**
 * @file
 * @brief Mock dependencies for client Message 4 process tests.
 */

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "edhoc/client/handshake/mocks/message_4/tst_cli_mock_edhoc_message_4_process.h"

#include <edhoc.h>

static int message_4_process_result = EDHOC_SUCCESS;

void tst_cli_edhoc_m4_process_reset_mock(void) {
  message_4_process_result = EDHOC_SUCCESS;
}

void tst_cli_edhoc_m4_process_set_failure(void) {
  message_4_process_result = EDHOC_ERROR_MSG_4_PROCESS_FAILURE;
}

void tst_cli_edhoc_m4_process_set_ok(void) {
  message_4_process_result = EDHOC_SUCCESS;
}

int __wrap_edhoc_message_4_process(  // NOLINT(*-reserved-identifier)
                                     // we need this
    struct edhoc_context* edhoc_context, const uint8_t* message_4,
    const size_t message_4_length) {
  (void)edhoc_context;
  (void)message_4;
  (void)message_4_length;
  return message_4_process_result;
}
