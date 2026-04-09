#ifndef COAP_CLIENT_CLI_LOG_ERROR_H_
#define COAP_CLIENT_CLI_LOG_ERROR_H_

#include <coap3/coap.h>
#include <stdint.h>

/**
 * @brief Parse and log EDHOC error payload received in a CoAP error response.
 *
 * @param[in] response_code CoAP response code associated with the payload.
 * @param[in] payload Raw response payload bytes.
 * @param[in] payload_len Number of payload bytes.
 */
void cp_cli_log_received_edhoc_error_response(coap_pdu_code_t response_code,
                                              const uint8_t* payload,
                                              size_t payload_len);

#endif  // COAP_CLIENT_CLI_LOG_ERROR_H_
