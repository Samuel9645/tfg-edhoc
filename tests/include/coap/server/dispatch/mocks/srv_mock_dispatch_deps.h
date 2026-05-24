/**
 * @file
 * @brief Linker-wrap helpers for dispatcher tests.
 */
#ifndef COAP_SERVER_DISPATCH_MOCKS_SRV_MOCK_DISPATCH_DEPS_H_
#define COAP_SERVER_DISPATCH_MOCKS_SRV_MOCK_DISPATCH_DEPS_H_

void tst_srv_set_calloc_failure(void);
void tst_srv_set_calloc_ok(void);

void tst_srv_set_setup_context_failure(void);

void tst_srv_dispatch_reset_mock(void);

#endif  // COAP_SERVER_DISPATCH_MOCKS_SRV_MOCK_DISPATCH_DEPS_H_
