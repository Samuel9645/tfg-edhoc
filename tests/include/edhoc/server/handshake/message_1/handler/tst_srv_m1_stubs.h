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
#include <stdint.h>

#include "common/data_models.h"

void tst_edh_srv_hnd_reset_stub_results(void);

void tst_edh_srv_m1_assert_handler_writes_error_payload(
    struct com_writable_buffer response);

extern int tst_edh_srv_stub_context_setup_result;
extern int tst_edh_srv_stub_message_1_process_result;
extern int tst_edh_srv_stub_message_2_compose_result;
extern const uint8_t* tst_edh_srv_stub_message_2_compose_output_buffer;
extern size_t tst_edh_srv_stub_message_2_compose_written_length;

#endif  // EDHOC_SERVER_HANDSHAKE_STUBS_H_