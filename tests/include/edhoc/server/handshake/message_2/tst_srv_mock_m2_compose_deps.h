/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Mocks to simulate libedhoc failures in the Message 2 composition
 * module
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_2_COMPOSE_TST_SRV_MOCK_M2_COMPOSE_DEPS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_2_COMPOSE_TST_SRV_MOCK_M2_COMPOSE_DEPS_H_

#include "common/com_data_models.h"

void tst_srv_edhoc_m2_compose_set_compose_failure(void);
void tst_srv_edhoc_m2_compose_set_compose_empty_length(void);
void tst_srv_edhoc_m2_compose_assert_writes_message_in_buffer(
    struct com_writable_buffer response);

void tst_srv_edhoc_m2_compose_reset_mock_results(void);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_2_COMPOSE_TST_SRV_MOCK_M2_COMPOSE_DEPS_H_