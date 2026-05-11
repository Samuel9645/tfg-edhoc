/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 27/04/2026
 * @brief Declarations for the default values of the edhoc_params struct in
 * tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_COMMON_TST_EDHOC_DEFAULT_PARAMS_H_
#define EDHOC_COMMON_TST_EDHOC_DEFAULT_PARAMS_H_

#include "edhoc/common/com_edhoc_parameters.h"

struct com_edhoc_parameters tst_edhoc_create_test_params(
    struct com_edhoc_cipher_suite_list suites, struct com_edhoc_methods methods,
    const struct com_edhoc_cipher_suite_details* selected_suite);

struct com_edhoc_parameters tst_edhoc_get_method_0_suite_0_params(void);

#endif  // EDHOC_COMMON_TST_EDHOC_DEFAULT_PARAMS_H_