/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 11/04/2026
 * @brief Mocked function edhoc_error_get_code
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppParameterMayBeConstPtrOrRef
#include <edhoc.h>

#include "edhoc/common/add_error/mocks/tst_mock_edhoc_get_code.h"

static int get_code_result = EDHOC_SUCCESS;
static bool use_real_get_code = false;

extern int __real_edhoc_error_get_code(  // NOLINT(*-reserved-identifier)
                                         // we need this
    const struct edhoc_context *context, enum edhoc_error_code *error_code);

void tst_com_edhoc_reset_get_code_mock(void) {
  get_code_result = EDHOC_SUCCESS;
  use_real_get_code = false;
}

void tst_com_edhoc_set_get_code_failed(void) {
  use_real_get_code = false;
  get_code_result = EDHOC_ERROR_BAD_STATE;
}

int __wrap_edhoc_error_get_code(  // NOLINT(*-reserved-identifier)
                                  // we need this
    const struct edhoc_context *edhoc_context,
    enum edhoc_error_code *error_code) {  // NOLINT(*-non-const-parameter)
  (void)edhoc_context;
  (void)error_code;
  if (use_real_get_code) {
    return __real_edhoc_error_get_code(edhoc_context, error_code);
  }
  *error_code = EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;
  return get_code_result;
}
