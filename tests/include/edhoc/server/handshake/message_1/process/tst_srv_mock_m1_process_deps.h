/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Mocks to simulate libedhoc failures in the Message 1 process module
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_PROCESS_TST_SRV_MOCK_M1_PROCESS_DEPS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_PROCESS_TST_SRV_MOCK_M1_PROCESS_DEPS_H_

void tst_srv_edhoc_m1_process_set_setup_failure(void);
void tst_srv_edhoc_m1_process_set_process_failure(void);
void tst_srv_edhoc_m1_process_reset_mock_results(void);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_PROCESS_TST_SRV_MOCK_M1_PROCESS_DEPS_H_