/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 11/04/2026
 * @brief Mocked function edhoc_error_get_code
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppParameterMayBeConst
#include <edhoc.h>

#include "edhoc/common/add_error/tst_mock_edhoc_get_code.h"

static int get_code_result = EDHOC_SUCCESS;

void srv_edhoc_reset_get_code_mock(void) { get_code_result = EDHOC_SUCCESS; }

void srv_edhoc_set_get_code_failed(void) {
  get_code_result = EDHOC_ERROR_BAD_STATE;
}

int edhoc_error_get_code(
    const struct edhoc_context *edhoc_context,
    enum edhoc_error_code *error_code) {  // NOLINT(*-non-const-parameter)
  (void)edhoc_context;
  (void)error_code;
  return get_code_result;
}
