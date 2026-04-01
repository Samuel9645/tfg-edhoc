#ifndef COAP_COMMON_RESPONSE_H_
#define COAP_COMMON_RESPONSE_H_

#include <coap3/coap.h>
#include <stdint.h>

#include "coap/common/status.h"

/**
 * @brief Add payload to response PDU.
 *
 * @param[in,out] response CoAP response PDU.
 * @param[in] payload Payload buffer.
 * @param[in] payload_len Payload length.
 * @return CCOM_STATUS_SUCCESS on success, CCOM_ERROR on
 * failure.
 */
coap_status_result_t coap_common_add_response_payload(coap_pdu_t* response,
                                                      const uint8_t* payload,
                                                      size_t payload_len);

#endif  // COAP_COMMON_RESPONSE_H_
