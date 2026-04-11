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
static bool use_real_compose = true;

extern int __real_edhoc_message_error_compose(  // NOLINT(*-reserved-identifier)
                                                // we need this
    uint8_t *message_error, size_t message_error_size,
    size_t *message_error_length, enum edhoc_error_code error_code,
    const struct edhoc_error_info *error_info);

void srv_edhoc_reset_error_compose_mock(void) {
  compose_result = EDHOC_SUCCESS;
  use_real_compose = false;
}

void srv_edhoc_use_real_compose(void) { use_real_compose = true; }

void srv_edhoc_set_error_compose_failed(void) {
  compose_result = EDHOC_ERROR_BAD_STATE;
  use_real_compose = false;
}

int __wrap_edhoc_message_error_compose(  // NOLINT(*-reserved-identifier)
                                         // we need this
    uint8_t *message_error, size_t message_error_size,
    size_t *message_error_length, enum edhoc_error_code error_code,
    const struct edhoc_error_info *error_info) {
  if (use_real_compose) {
    return __real_edhoc_message_error_compose(message_error, message_error_size,
                                              message_error_length, error_code,
                                              error_info);
  }

  if (compose_result == EDHOC_SUCCESS && message_error_length != NULL) {
    *message_error_length = 1;
  }
  return compose_result;
}