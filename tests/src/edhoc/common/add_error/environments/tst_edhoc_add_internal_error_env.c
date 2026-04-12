/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 12/04/2026
 * @brief Declarations for the tst_edhoc_add_error_env module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/common/add_error/environments/tst_edhoc_add_internal_error_env.h"

#include <string.h>

void tst_edhoc_add_internal_error_reset_env(
    struct tst_edhoc_add_internal_error_env* env) {
  memset(env->error_message, 0, sizeof(env->error_message));
  env->error_buffer_view.length = 0;
  env->error_buffer_view.bytes = env->error_message;
}
