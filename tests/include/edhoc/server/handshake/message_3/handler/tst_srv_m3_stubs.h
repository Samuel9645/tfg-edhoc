/**
 * @file stubs.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Stubs for EDHOC server message 3 handler tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_STUBS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_STUBS_H_

#include <stddef.h>
#include <stdint.h>

void tst_edh_srv_message_3_reset_stub_results(void);

void tst_edh_srv_message_3_set_stub_error_response(const uint8_t* data,
                                                   size_t length);

extern int tst_edh_srv_message_3_stub_edhoc_process_result;
extern int tst_edh_srv_message_3_stub_edhoc_compose_result;

extern const uint8_t TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_PAYLOAD[];
extern const size_t TST_EDH_SRV_MESSAGE_3_MOCK_ERROR_LEN;

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_STUBS_H_
