/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 08/04/2026
 * @brief Message 1 specific EDHOC error-response composition testing.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_ERRORS_TST_SRV_M1_ERRORS_STUBS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_ERRORS_TST_SRV_M1_ERRORS_STUBS_H_
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

extern bool tst_stub_get_suites_called;
extern int tst_stub_get_cipher_suites_result;
extern const int32_t* tst_stub_get_cipher_suites;
extern size_t tst_stub_get_cipher_suites_length;

void tst_srv_m1_err_reset_stub_results(void);
#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_ERRORS_TST_SRV_M1_ERRORS_STUBS_H_