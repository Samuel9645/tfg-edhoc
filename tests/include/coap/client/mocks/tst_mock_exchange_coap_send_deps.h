
#ifndef COAP_MOCKS_TST_MOCK_COAP_SEND_H_
#define COAP_MOCKS_TST_MOCK_COAP_SEND_H_

#include "coap/client/cli_exchange.h"

enum status_coap cli_coap_mock_send(
    struct cli_coap_exchange* exchange,
    struct cli_coap_exchange_request request_data);

struct cli_coap_exchange_request tst_mock_coap_get_last_sent_data(void);

void tst_cli_prepend_reset_all_mocks(void);

void tst_cli_prepend_flow_set_failure(void);
void tst_cli_prepend_flow_reset_mock(void);

void tst_cli_recalculate_size_set_failure(void);
void tst_cli_recalculate_size_reset_mock(void);

void tst_cli_prepend_connection_id_set_failure(void);
void tst_cli_prepend_connection_id_reset_mock(void);
#endif  // COAP_MOCKS_TST_MOCK_COAP_SEND_H_