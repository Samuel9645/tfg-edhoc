/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 12/05/2026
 * @brief Module to extract EDHOC Message 3 from a CoAP request payload.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef COAP_SERVER_EXTRACT_EDHOC_MESSAGE_SRV_COAP_EXTRACT_M3_H_
#define COAP_SERVER_EXTRACT_EDHOC_MESSAGE_SRV_COAP_EXTRACT_M3_H_

#include <coap3/coap.h>
#include <edhoc.h>

#include "common/com_data_models.h"

enum srv_coap_extract_message_3_status {
  SRV_COAP_EXTRACT_MSG3_OK = 0,
  SRV_COAP_EXTRACT_MSG3_ERR_INVALID_ERROR_BUFFER,
  SRV_COAP_EXTRACT_MSG3_ERR_EMPTY_REQUEST_BUFFER,
  SRV_COAP_EXTRACT_MSG3_ERR_NULL_EDHOC_CONTEXT,
  SRV_COAP_EXTRACT_MSG3_ERR_CON_ID_EXTRACTION_FAILED,
  SRV_COAP_EXTRACT_MSG3_ERR_UNEXPECTED_CONNECTION_ID,
};

struct srv_coap_extract_message_3_result {
  const enum srv_coap_extract_message_3_status status;
  const struct com_readonly_buffer buffer;
};

/**
 * @brief Extracts EDHOC Message 3 from the raw request buffer and validates
 * the connection ID against the provided EDHOC context.
 * @param[in] request_buffer Input buffer containing the request payload.
 * @param[in] edhoc_ctx EDHOC context used to validate the connection ID.
 * @param[in] error_response Metadata of the buffer (pointer and capacity) to
 * write into.
 * @return Result struct containing the extracted Message 3 payload on success,
 * or an error code and encoded error buffer on failure.
 */
struct srv_coap_extract_message_3_result srv_coap_extract_message_3(
    struct com_readonly_buffer request_buffer,
    const struct edhoc_context* edhoc_ctx,
    struct com_writable_buffer error_response);

coap_pdu_code_t srv_coap_map_extract_message_3_to_pdu_code(
    enum srv_coap_extract_message_3_status status);

bool srv_edhoc_is_message_3(struct com_readonly_buffer request_buffer,
                            const struct edhoc_context* context);

#endif  // COAP_SERVER_EXTRACT_EDHOC_MESSAGE_SRV_COAP_EXTRACT_M3_H_

