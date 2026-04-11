/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 08/04/2026
 * @brief Mocks of the error composition functions for testing error handling in
 * EDHOC server handshake
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_COMMON_TST_MOCK_ADD_EDHOC_ERROR_INFO_H_
#define EDHOC_COMMON_TST_MOCK_ADD_EDHOC_ERROR_INFO_H_

#include "common/com_data_models.h"

void tst_srv_edhoc_write_mock_buffer_to_response(
    struct com_writable_buffer* response_data);

void tst_srv_edhoc_assert_handler_writes_error_in_buffer(
    struct com_writable_buffer response);

#endif  // EDHOC_COMMON_TST_MOCK_ADD_EDHOC_ERROR_INFO_H_