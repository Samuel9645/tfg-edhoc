/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Mock of the EDHOC message 1 process function of libedhoc for testing
 * the Message 1 process module.
 * @see [libedhoc
 * documentation](https://kamil-kielbasa.github.io/libedhoc/index.html)
 * @see [RFC
 * 9528 5.2.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-processing-of-mes)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_MOCKS_TST_SRV_MOCK_EDHOC_MESSAGE_1_PROCESS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_MOCKS_TST_SRV_MOCK_EDHOC_MESSAGE_1_PROCESS_H_

void tst_srv_edhoc_m1_set_process_failure(void);
void tst_srv_edhoc_m1_set_process_ok(void);
void tst_srv_edhoc_m1_use_real_process(void);

void tst_srv_edhoc_m1_reset_process_mock(void);
#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_MOCKS_TST_SRV_MOCK_EDHOC_MESSAGE_1_PROCESS_H_