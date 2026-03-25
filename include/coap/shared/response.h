#ifndef COAP_SHARED_RESPONSE_H_
#define COAP_SHARED_RESPONSE_H_

#include <coap3/coap.h>
#include <stddef.h>
#include <stdint.h>

#include "coap/coap_config.h"
#include "coap/common/status.h"

/**
 * @brief Add EDHOC content-format options to response PDU.
 *
 * @param[in,out] response CoAP response PDU.
 * @param[in] content_format Content format to set in options.
 * @return COAP_STATUS_SUCCESS on success, COAP_STATUS_ERROR on failure.
 */
coap_status_result_t coap_shared_add_edhoc_response_options(
    coap_pdu_t* response, content_format_edhoc_values_t content_format);

/**
 * @brief Add payload to response PDU.
 *
 * @param[in,out] response CoAP response PDU.
 * @param[in] payload Payload buffer.
 * @param[in] payload_len Payload length.
 * @return COAP_STATUS_SUCCESS on success, COAP_STATUS_ERROR on failure.
 */
coap_status_result_t coap_shared_add_response_payload(coap_pdu_t* response,
                                                      const uint8_t* payload,
                                                      size_t payload_len);

#endif // COAP_SHARED_RESPONSE_H_
