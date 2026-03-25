#ifndef COAP_SHARED_EDHOC_REQUEST_H_
#define COAP_SHARED_EDHOC_REQUEST_H_

#include <coap3/coap.h>
#include <edhoc_helpers.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "coap/common/config.h"
#include "coap/common/status.h"

/**
 * @brief Validate EDHOC CoAP request content format and extract payload.
 *
 * @param[in] request Incoming CoAP request.
 * @param[in] expected_format Expected content format value.
 * @param[out] payload Pointer to request payload on success.
 * @param[out] payload_len Payload length on success.
 * @return COAP_STATUS_SUCCESS on success, COAP_STATUS_ERROR on failure.
 */
coap_status_result_t coap_shared_extract_payload_if_valid_edhoc_request(
    const coap_pdu_t* request, content_format_edhoc_values_t expected_format,
    const uint8_t** payload, size_t* payload_len);

/**
 * @brief Check whether payload is properly formatted as EDHOC Message 1.
 *
 * @param[in] payload Request payload.
 * @param[in] payload_len Request payload length.
 * @return true if payload matches Message 1 framing, false otherwise.
 */
bool coap_shared_is_properly_formatted_message_1(const uint8_t* payload,
                                                 size_t payload_len);

/**
 * @brief Check whether payload is properly formatted as EDHOC Message 3.
 *
 * Validates that the payload contains a properly formatted Message 3 with
 * correct connection ID. Extracts connection ID fields for further processing.
 *
 * @param[in] request_payload Request payload.
 * @param[in] request_len Request payload length.
 * @param[in] edhoc_ctx EDHOC context with private connection ID.
 * @param[out] extracted_fields Populated with extracted message fields on
 * success.
 * @return true if payload is valid Message 3, false otherwise.
 */
bool coap_shared_is_properly_formatted_message_3(
    const uint8_t* request_payload, size_t request_len,
    const struct edhoc_context* edhoc_ctx,
    struct edhoc_extracted_fields* extracted_fields);

#endif  // COAP_SHARED_EDHOC_REQUEST_H_
