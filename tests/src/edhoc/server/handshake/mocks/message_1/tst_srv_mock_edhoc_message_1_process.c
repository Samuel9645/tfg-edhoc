/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Mock of the EDHOC message 1 process function of libedhoc for testing
 * the Message 1 process module.
 * @see [libedhoc
 * documentation](https://kamil-kielbasa.github.io/libedhoc/index.html)
 * @see [RFC
 * 9528 5.2.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-processing-of-mes)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/mocks/message_1/tst_srv_mock_edhoc_message_1_process.h"

#include <edhoc.h>
#include <stdbool.h>

static bool use_real_edhoc_message_1_process = false;
static int message_1_process_result = EDHOC_SUCCESS;

void tst_srv_edhoc_m1_set_process_failure(void) {
  use_real_edhoc_message_1_process = false;
  message_1_process_result = EDHOC_ERROR_MSG_1_PROCESS_FAILURE;
}

void tst_srv_edhoc_m1_set_process_ok(void) {
  use_real_edhoc_message_1_process = false;
  message_1_process_result = EDHOC_SUCCESS;
}

void tst_srv_edhoc_m1_reset_process_mock(void) {
  use_real_edhoc_message_1_process = false;
  message_1_process_result = EDHOC_SUCCESS;
}

extern int __real_edhoc_message_1_process(  // NOLINT(*-reserved-identifier)
                                            // we need this
    struct edhoc_context* edhoc_context, const uint8_t* message_1,
    size_t message_1_length);

int __wrap_edhoc_message_1_process(  // NOLINT(*-reserved-identifier)
                                     // we need this
    struct edhoc_context* edhoc_context, const uint8_t* message_1,
    const size_t message_1_length) {
  if (use_real_edhoc_message_1_process) {
    return __real_edhoc_message_1_process(edhoc_context, message_1,
                                          message_1_length);
  }
  return message_1_process_result;
}
