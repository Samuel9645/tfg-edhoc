/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/05/2026
 * @brief Mocking of the `edhoc_process_error` function for testing purposes.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <edhoc.h>

#include "edhoc/server/handshake/mocks/message_2/tst_srv_mock_edhoc_message_2_compose.h"
static bool use_real_process_error = true;

void tst_cli_edhoc_set_process_error_failure(void) {
  use_real_process_error = false;
}

void tst_cli_edhoc_reset_process_error_mock(void) {
  use_real_process_error = true;
}

extern int __real_edhoc_message_error_process(  // NOLINT(*-reserved-identifier)
                                                // we need this
    const uint8_t* message_error, size_t message_error_length,
    enum edhoc_error_code* error_code, struct edhoc_error_info* error_info);

int __wrap_edhoc_message_error_process(  // NOLINT(*-reserved-identifier)
                                         // we need this
    const uint8_t* message_error, const size_t message_error_length,
    enum edhoc_error_code* error_code, struct edhoc_error_info* error_info) {
  if (use_real_process_error) {
    return __real_edhoc_message_error_process(
        message_error, message_error_length, error_code, error_info);
  }
  return EDHOC_ERROR_NOT_PERMITTED;
}