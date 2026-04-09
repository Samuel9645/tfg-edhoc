/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 08/04/2026
 * @brief Helpers for common operations on response buffers in tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_COMMON_TST_RESPONSE_BUFFER_HELPERS_H_
#define EDHOC_COMMON_TST_RESPONSE_BUFFER_HELPERS_H_

#include "common/sys_data_models.h"

bool edh_com_response_buffer_is_null(const struct com_writable_buffer* buffer);

#endif  // EDHOC_COMMON_TST_RESPONSE_BUFFER_HELPERS_H_