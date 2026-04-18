/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 12/04/2026
 * @brief Custom assertions for the add edhoc error tests
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_COMMON_ADD_ERROR_COMMON_TST_ASSERTIONS_H_
#define EDHOC_COMMON_ADD_ERROR_COMMON_TST_ASSERTIONS_H_

#include <edhoc.h>

#include "common/com_data_models.h"
#include "edhoc/common/add_error/com_edhoc_add_error_status.h"

void tst_edhoc_assert_add_error_status_ok(
    struct com_edhoc_add_error_result add_error_result);

void tst_edhoc_assert_add_error_status_ok_with_message(
    struct com_edhoc_add_error_result add_error_result, const char* message);

void tst_edhoc_assert_encoded_error_matches(
    struct com_readonly_buffer encoded_error_buffer,
    const char* expected_error_description,
    enum edhoc_error_code expected_error_code);

void tst_edhoc_assert_encoded_error_is_not_empty(
    struct com_readonly_buffer encoded_error_buffer);

void tst_edhoc_assert_error_not_empty_if_present(
    struct com_readonly_buffer response);

#endif  // EDHOC_COMMON_ADD_ERROR_COMMON_TST_ASSERTIONS_H_