/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Stubs for EDHOC server message 3 handler tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_STUBS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_STUBS_H_

#include "common/com_data_models.h"

void tst_srv_edhoc_m3_assert_handler_writes_message_4_in_buffer(
    struct com_writable_buffer response);

void tst_srv_edhoc_m1_set_message_3_process_failure(void);
void tst_srv_edhoc_m1_set_message_4_compose_failure(void);
void tst_srv_edhoc_m3_set_message_4_compose_empty_length(void);

void tst_srv_edhoc_m3_reset_stub_results(void);
#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_STUBS_H_
