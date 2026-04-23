/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 23/04/2026
 * @brief Mock dependencies for client Message 2 process tests.
 */

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "edhoc/client/handshake/mocks/message_2/tst_cli_mock_edhoc_message_2_process.h"

#include <edhoc.h>

static int message_2_process_result = EDHOC_SUCCESS;

void tst_cli_edhoc_m2_process_reset_mock(void) {
  message_2_process_result = EDHOC_SUCCESS;
}

void tst_cli_edhoc_m2_process_set_failure(void) {
  message_2_process_result = EDHOC_ERROR_MSG_2_PROCESS_FAILURE;
}

void tst_cli_edhoc_m2_process_set_ok(void) {
  message_2_process_result = EDHOC_SUCCESS;
}

int __wrap_edhoc_message_2_process(  // NOLINT(*-reserved-identifier)
                                     // we need this

    struct edhoc_context* edhoc_context, const uint8_t* message_2,
    const size_t message_2_length) {
  (void)edhoc_context;
  (void)message_2;
  (void)message_2_length;
  return message_2_process_result;
}
