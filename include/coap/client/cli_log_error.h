#ifndef COAP_CLIENT_CLI_LOG_ERROR_H_
#define COAP_CLIENT_CLI_LOG_ERROR_H_

#include <coap3/coap.h>

#include "common/com_data_models.h"

/**
 * @brief Parse and log EDHOC error payload received in a CoAP error response.
 *
 * @param[in] response_code CoAP response code associated with the payload.
 * @param[in] error View of the error buffer
 */
void cli_coap_log_received_edhoc_error_response(
    coap_pdu_code_t response_code, struct com_readonly_buffer error);

#endif  // COAP_CLIENT_CLI_LOG_ERROR_H_
