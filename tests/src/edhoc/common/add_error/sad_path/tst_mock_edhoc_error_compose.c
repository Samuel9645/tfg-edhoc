/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 11/04/2026
 * @brief Mocked function edhoc_message_error_compose
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppParameterMayBeConst
#include "edhoc/common/add_error/tst_mock_edhoc_error_compose.h"

#include <edhoc.h>

static int compose_result = EDHOC_SUCCESS;

void srv_edhoc_reset_error_compose_mock(void) {
  compose_result = EDHOC_SUCCESS;
}

void srv_edhoc_set_error_compose_failed(void) {
  compose_result = EDHOC_ERROR_BAD_STATE;
}

int edhoc_message_error_compose(
    uint8_t *message_error,            // NOLINT(*-non-const-parameter)
    size_t message_error_size,         // NOLINT(*-non-const-parameter)
    size_t *message_error_length,      // NOLINT(*-non-const-parameter)
    enum edhoc_error_code error_code,  // NOLINT(*-non-const-parameter)
    const struct edhoc_error_info *error_info) {
  (void)message_error;
  (void)message_error_length;
  (void)message_error_size;
  (void)error_code;
  (void)error_info;
  return compose_result;
}
