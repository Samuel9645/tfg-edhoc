#ifndef COAP_COMMON_COAP_RESPONSE_H_
#define COAP_COMMON_COAP_RESPONSE_H_

#include <coap3/coap.h>

#include "coap/coap_config.h"
#include "coap/common/com_coap_status.h"
/**
 * @brief Add payload to response PDU.
 *
 * @param[in,out] response CoAP response PDU.
 * @param[in] payload Payload buffer.
 * @param[in] payload_len Payload length.
 * @return CP_STATUS_SUCCESS on success, CP_STATUS_FAILURE on failure
 */
enum status_coap com_coap_add_response_payload(coap_pdu_t* response,
                                               const uint8_t* payload,
                                               size_t payload_len);

enum status_coap com_coap_add_edhoc_response_options(
    coap_pdu_t* response,
    enum config_coap_content_format_edhoc_values content_format);

#endif  // COAP_COMMON_COAP_RESPONSE_H_
