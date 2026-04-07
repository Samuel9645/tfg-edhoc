/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Stubs for EDHOC server message 3 handler tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_STUBS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_STUBS_H_

#include <stddef.h>
#include <stdint.h>

#include "common/data_models.h"

void tst_edh_srv_message_3_reset_stub_results(void);

void tst_edh_srv_message_3_set_stub_error_response(const uint8_t* data,
                                                   size_t length);
void tst_edh_srv_m3_assert_handler_writes_error_payload(
    struct com_writable_buffer response);

extern int tst_edh_srv_stub_process_message_3_result;
extern int tst_edh_srv_stub_message_4_compose_result;
extern size_t tst_edh_srv_stub_message_4_compose_written_length;

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_STUBS_H_
