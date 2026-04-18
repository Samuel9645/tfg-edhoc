/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Mock of the EDHOC message 2 compose function of libedhoc for testing
 * the Message 2 compose module.
 * @see [libedhoc
 * documentation](https://kamil-kielbasa.github.io/libedhoc/index.html)
 * @see [RFC
 * 9528 5.3.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-composition-of-me
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_MOCKS_TST_SRV_MOCK_EDHOC_MESSAGE_2_COMPOSE_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_MOCKS_TST_SRV_MOCK_EDHOC_MESSAGE_2_COMPOSE_H_

#include "common/com_data_models.h"

void tst_srv_edhoc_m2_set_compose_failure(void);
void tst_srv_edhoc_m2_set_compose_ok(void);
void tst_srv_edhoc_m2_compose_set_compose_empty_length(void);
void tst_srv_edhoc_m2_compose_assert_writes_message_in_buffer(
    struct com_readonly_buffer response);

void tst_srv_edhoc_m2_reset_compose_mock(void);
#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_MOCKS_TST_SRV_MOCK_EDHOC_MESSAGE_2_COMPOSE_H_