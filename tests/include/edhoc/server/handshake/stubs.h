/**
 * @file stubs.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Declarations of the variables used in the libedhoc stubs
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_STUBS_H_
#define EDHOC_SERVER_HANDSHAKE_STUBS_H_

#include <stddef.h>
#include <stdint.h>

void tst_edh_srv_hnd_reset_stub_results(void);

void tst_edh_srv_hnd_set_stub_error_response(const uint8_t* data, size_t len);

extern int tst_edh_srv_hnd_stub_edhoc_setup_res;
extern int tst_edh_srv_hnd_stub_edhoc_process_res;
extern int tst_edh_srv_hnd_stub_edhoc_compose_res;

extern const uint8_t TST_EDH_SRV_HND_MOCK_ERROR_PAYLOAD[];
extern const size_t TST_EDH_SRV_HND_MOCK_ERROR_LEN;

#endif  // EDHOC_SERVER_HANDSHAKE_STUBS_H_