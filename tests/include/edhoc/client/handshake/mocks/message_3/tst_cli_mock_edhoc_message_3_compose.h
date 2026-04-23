/**
 * @file
 * @author Samuel Rodriguez <alu0101545714@ull.edu.es>
 * @since 23/04/2026
 * @brief Mock dependencies for client Message 3 compose tests.
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MOCKS_MESSAGE_3_TST_CLI_MOCK_EDHOC_MESSAGE_3_COMPOSE_H_
#define EDHOC_CLIENT_HANDSHAKE_MOCKS_MESSAGE_3_TST_CLI_MOCK_EDHOC_MESSAGE_3_COMPOSE_H_

#include "common/com_data_models.h"

void tst_cli_edhoc_m3_compose_reset_mock(void);
void tst_cli_edhoc_m3_compose_set_compose_ok(void);
void tst_cli_edhoc_m3_compose_set_prepend_connection_id_failure(void);
void tst_cli_edhoc_m3_compose_set_compose_failure(void);
void tst_cli_edhoc_m3_compose_set_compose_empty_length(void);
void tst_cli_edhoc_m3_compose_set_recalculate_size_failure(void);
void tst_cli_edhoc_m3_compose_assert_writes_message_in_buffer(
    struct com_readonly_buffer message_3);

#endif  // EDHOC_CLIENT_HANDSHAKE_MOCKS_MESSAGE_3_TST_CLI_MOCK_EDHOC_MESSAGE_3_COMPOSE_H_
