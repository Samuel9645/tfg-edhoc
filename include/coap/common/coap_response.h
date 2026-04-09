#ifndef COAP_COMMON_RESPONSE_H_
#define COAP_COMMON_RESPONSE_H_

#include <coap3/coap.h>

#include "coap/common/coap_status.h"

/**
 * @brief Add payload to response PDU.
 *
 * @param[in,out] response CoAP response PDU.
 * @param[in] payload Payload buffer.
 * @param[in] payload_len Payload length.
 * @return CP_STATUS_SUCCESS on success, CP_STATUS_FAILURE on failure
 */
enum cp_status cp_com_add_response_payload(coap_pdu_t* response,
                                           const uint8_t* payload,
                                           size_t payload_len);

#endif  // COAP_COMMON_RESPONSE_H_
