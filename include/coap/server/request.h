/**
 * @file request.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Contains the functions related to CoAP request
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef COAP_SERVER_REQUEST_H_
#define COAP_SERVER_REQUEST_H_

#include <coap3/coap.h>
#include <stdint.h>

#include "coap/common/status.h"
#include "coap/config.h"

/**
 * @brief Validate EDHOC CoAP request content format and extract payload.
 *
 * @param[in] request Incoming CoAP request.
 * @param[in] expected_format Expected content format value.
 * @param[out] payload Pointer to request payload on success.
 * @param[out] payload_len Payload length on success.
 * @return CP_STATUS_SUCCESS if request is valid and payload is extracted,
 * CP_STATUS_ERROR otherwise.
 */
cp_status_t cp_srv_extract_payload_if_valid_edhoc_request(
    const coap_pdu_t* request,
    cp_cfg_content_format_edhoc_values_t expected_format,
    const uint8_t** payload, size_t* payload_len);

#endif  // COAP_SERVER_REQUEST_H_