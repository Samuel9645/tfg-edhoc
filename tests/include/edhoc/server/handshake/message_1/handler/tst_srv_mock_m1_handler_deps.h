/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Declarations of the variables used in the libedhoc stubs
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_STUBS_H_
#define EDHOC_SERVER_HANDSHAKE_STUBS_H_

#include "common/com_data_models.h"

void tst_edh_srv_m1_assert_handler_writes_message_2_in_buffer(
    struct com_writable_buffer response);

void tst_edh_srv_m1_set_setup_failure(void);
void tst_edh_srv_m1_set_message_1_process_failure(void);
void tst_edh_srv_m1_set_message_2_compose_failure(void);
void tst_edh_srv_m1_set_message_2_compose_empty_length(void);

void tst_edh_srv_m1_reset_stub_results(void);
#endif  // EDHOC_SERVER_HANDSHAKE_STUBS_H_