/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Declarations of the variables used in the libedhoc stubs
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_STUBS_H_
#define EDHOC_SERVER_HANDSHAKE_STUBS_H_

#include <stddef.h>

#include "common/data_models.h"

void tst_edh_srv_hnd_reset_stub_results(void);

void tst_edh_srv_m1_assert_handler_writes_message_2_in_buffer(
    struct com_writable_buffer response);

void tst_edh_srv_m1_configure_behavior(int setup_res, int process_res,
                                       int compose_res);

void tst_edh_srv_m1_set_compose_length(size_t length);

#endif  // EDHOC_SERVER_HANDSHAKE_STUBS_H_