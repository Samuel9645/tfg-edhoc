/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 29/04/2026
 * @brief Mock of the EDHOC setup context function and its dependencies for
 * testing the setup context module.
 * @see [libedhoc
 * documentation](https://kamil-kielbasa.github.io/libedhoc/index.html)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_COMMON_SETUP_CONTEXT_MOCKS_TST_COM_MOCK_EDHOC_SETUP_CONTEXT_H_
#define EDHOC_COMMON_SETUP_CONTEXT_MOCKS_TST_COM_MOCK_EDHOC_SETUP_CONTEXT_H_

void tst_com_set_psa_crypto_init_failure(void);
void tst_com_use_real_psa_crypto_init(void);

void tst_com_set_edhoc_context_init_failure(void);
void tst_com_use_real_edhoc_context_init(void);

void tst_com_set_edhoc_set_methods_failure(void);
void tst_com_use_real_edhoc_set_methods(void);

void tst_com_set_edhoc_set_cipher_suites_failure(void);
void tst_com_use_real_edhoc_set_cipher_suites(void);

void tst_com_set_edhoc_set_connection_id_failure(void);
void tst_com_use_real_edhoc_set_connection_id(void);

void tst_com_set_edhoc_bind_keys_failure(void);
void tst_com_use_real_edhoc_bind_keys(void);

void tst_com_set_edhoc_bind_crypto_failure(void);
void tst_com_use_real_edhoc_bind_crypto(void);

void tst_com_set_edhoc_bind_credentials_failure(void);
void tst_com_use_real_edhoc_bind_credentials(void);

void tst_com_reset_setup_context_mock(void);

#endif  // EDHOC_COMMON_SETUP_CONTEXT_MOCKS_TST_COM_MOCK_EDHOC_SETUP_CONTEXT_H_
