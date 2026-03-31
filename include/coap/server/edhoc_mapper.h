#ifndef COAP_SERVER_EDHOC_MAPPER_H_
#define COAP_SERVER_EDHOC_MAPPER_H_

#include <coap3/coap.h>
#include <edhoc.h>

#include "common/data_models.h"
#include "edhoc/server/handshake/message_1_result.h"
// TODO: DELETE THIS ENUM AND THE BELLOW FUNCTION
/**
 * @brief Classifies EDHOC failure source for CoAP response mapping.
 */
typedef enum {
  COAP_SERVER_EDHOC_PROTOCOL_ERROR,
  COAP_SERVER_EDHOC_INTERNAL_ERROR,
} coap_server_edhoc_failure_type_t;

// TODO: introduce structs to avoid too many parameters in this function
/**
 * @brief Map EDHOC failure to CoAP response code and compose error message.
 *
 * Generates an EDHOC error message in the response payload according to
 * RFC 9528 A.2.3. Sets CoAP response code to 4.00 (Bad Request) for protocol
 * errors or 5.00 (Internal Server Error) for internal failures.
 *
 * @param[in] edhoc_ctx EDHOC context carrying the failure details.
 * @param[in] operation_label Label used in error logs.
 * @param[in] failure_type Failure classification used to select 4.00 (protocol)
 * or 5.00 (internal).
 * @param[in] edhoc_api_result Return value from the failing libedhoc API call
 * (negative error code from edhoc_values.h).
 * @param[in,out] response CoAP response PDU where code is set.
 * @param[in,out] response_data Buffer where EDHOC error message is written.
 * @return Mapped CoAP response code.
 *
 * @note Sets response->code and populates response_data.payload with the
 * EDHOC error message. Caller must still set Content-Format option before
 * sending response.
 *
 * @see [RFC 9528 A.2.3 - Errors in EDHOC over
 * CoAP](https://datatracker.ietf.org/doc/html/rfc9528/#name-errors-in-edhoc-over-coap)
 */
coap_pdu_code_t coap_server_map_edhoc_failure_to_response(
    struct edhoc_context* edhoc_ctx, const char* operation_label,
    coap_server_edhoc_failure_type_t failure_type, int edhoc_api_result,
    coap_pdu_t* response, common_response_buffer_t* response_data);

/**
 * @brief Map EDHOC server handshake result to CoAP response code and set
 * session app-data on success.
 * @param message_1_result Result of Message 1 processing, including status and
 * allocated EDHOC context.
 * @param session CoAP session to set app-data on success.
 * @return Mapped CoAP response code: 2.04 (Changed) on success, 4.00 (Bad
 * Request) or 5.00 (Internal Server Error) on failure.
 */
coap_pdu_code_t coap_server_process_message_1_result(
    edhoc_server_message_1_result_t message_1_result, coap_session_t* session);

#endif  // COAP_SERVER_EDHOC_MAPPER_H_
