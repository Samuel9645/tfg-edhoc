/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 12/04/2026
 * @brief Declarations for the tst_edhoc_add_error_env module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_COMMON_ADD_ERROR_ENVIRONMENTS_TST_EDHOC_ADD_INTERNAL_ERROR_ENV_H_
#define EDHOC_COMMON_ADD_ERROR_ENVIRONMENTS_TST_EDHOC_ADD_INTERNAL_ERROR_ENV_H_
#include <stdint.h>

#include "common/com_data_models.h"
#include "edhoc/common/add_error/common/tst_edhoc_add_error_capacity.h"

struct tst_edhoc_add_internal_error_env {
  uint8_t error_message[TST_EDHOC_ADD_ERROR_CAPACITY];
  struct com_writable_buffer error_buffer_view;
};

void tst_edhoc_add_internal_error_reset_env(
    struct tst_edhoc_add_internal_error_env* env);

#endif  // EDHOC_COMMON_ADD_ERROR_ENVIRONMENTS_TST_EDHOC_ADD_INTERNAL_ERROR_ENV_H_