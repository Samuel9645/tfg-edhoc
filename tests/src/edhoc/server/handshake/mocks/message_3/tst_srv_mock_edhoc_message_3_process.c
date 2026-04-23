/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Mock of the EDHOC message 3 process function of libedhoc for testing
 * the Message 3 process module.
 * @see [libedhoc
 * documentation](https://kamil-kielbasa.github.io/libedhoc/index.html)
 * @see [RFC
 * 9528 5.2.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-processing-of-mes)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "edhoc/server/handshake/mocks/message_3/tst_srv_mock_edhoc_message_3_process.h"

#include <edhoc.h>

static int message_3_process_result = EDHOC_SUCCESS;

void tst_srv_edhoc_m3_set_process_failure(void) {
  message_3_process_result = EDHOC_ERROR_GENERIC_ERROR;
}

void tst_srv_edhoc_m3_set_process_ok(void) {
  message_3_process_result = EDHOC_SUCCESS;
}

void tst_srv_edhoc_m3_reset_process_mock(void) {
  message_3_process_result = EDHOC_SUCCESS;
}

extern int __real_edhoc_message_3_process(  // NOLINT(*-reserved-identifier)
                                            // we need this
    struct edhoc_context* edhoc_context, const uint8_t* message_3,
    size_t message_3_length);

int __wrap_edhoc_message_3_process(  // NOLINT(*-reserved-identifier)
                                     // we need this
    struct edhoc_context* edhoc_context, const uint8_t* message_3,
    const size_t message_3_length) {
  (void)edhoc_context;
  (void)message_3;
  (void)message_3_length;
  return message_3_process_result;
}
