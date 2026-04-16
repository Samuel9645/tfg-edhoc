/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Mock of the EDHOC context setup function of libedhoc for testing the
 * Message 1 process module.
 * @see [libedhoc
 * documentation](https://kamil-kielbasa.github.io/libedhoc/index.html)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_MOCKS_TST_SRV_MOCK_EDHOC_CONTEXT_INIT_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_MOCKS_TST_SRV_MOCK_EDHOC_CONTEXT_INIT_H_

void tst_srv_edhoc_m1_set_context_init_failure(void);

void tst_srv_edhoc_m1_reset_context_init_mock(void);
#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_MOCKS_TST_SRV_MOCK_EDHOC_CONTEXT_INIT_H_