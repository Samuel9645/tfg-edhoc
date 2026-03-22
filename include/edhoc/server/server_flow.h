#ifndef EDHOC_SERVER_SERVER_FLOW_H_
#define EDHOC_SERVER_SERVER_FLOW_H_

#include <coap3/coap.h>
#include <edhoc_helpers.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Handle EDHOC Message 1 and compose Message 2.
 *
 * @param[in] session CoAP session carrying EDHOC app-data state.
 * @param[in] request_payload Request payload bytes.
 * @param[in] request_len Request payload size.
 * @param[in] response_capacity Capacity of response_payload buffer.
 * @param[in,out] response CoAP response for status/error mapping.
 * @param[out] response_payload Buffer for composed Message 2.
 * @param[out] response_len Number of bytes written to response_payload.
 * @return CoAP response code for the operation result.
 *
 * @warning Allocates EDHOC context with calloc() and registers as session
 * app-data with free() as destructor. Caller must ensure the session is
 * properly cleaned up before application exit.
 */
coap_pdu_code_t server_edhoc_handle_message_1(
    coap_session_t* session, const uint8_t* request_payload, size_t request_len,
    size_t response_capacity, coap_pdu_t* response, uint8_t* response_payload,
    size_t* response_len);

/**
 * @brief Handle EDHOC Message 3 and compose Message 4.
 *
 * @param[in] session CoAP session carrying EDHOC app-data state.
 * @param[in] request_payload Request payload bytes.
 * @param[in] request_len Request payload size.
 * @param[in] response_capacity Capacity of response_payload buffer.
 * @param[in] extracted_fields Pre-validated and extracted Message 3 fields.
 * @param[in,out] response CoAP response for status/error mapping.
 * @param[out] response_payload Buffer for composed Message 4.
 * @param[out] response_len Number of bytes written to response_payload.
 * @return CoAP response code for the operation result.
 */
coap_pdu_code_t server_edhoc_handle_message_3(
    coap_session_t* session, const uint8_t* request_payload, size_t request_len,
    size_t response_capacity, struct edhoc_extracted_fields* extracted_fields,
    coap_pdu_t* response, uint8_t* response_payload, size_t* response_len);

#endif  // EDHOC_SERVER_SERVER_FLOW_H_
