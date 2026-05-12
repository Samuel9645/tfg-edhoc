/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
* @brief Module to extract the Message 1 from the request with the CBOR true
 * prefix
*  * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [RFC 9528
 * A.2](https://datatracker.ietf.org/doc/html/rfc9528#name-transferring-edhoc-over-coa)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PARSER_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PARSER_H_

#include <coap3/coap.h>

#include "common/com_data_models.h"

enum srv_coap_extract_message_1_status {
  SRV_COAP_EXTRACT_MSG1_OK = 0,
  SRV_COAP_EXTRACT_MSG1_ERR_INVALID_ERROR_BUFFER,
  SRV_COAP_EXTRACT_MSG1_ERR_EMPTY_REQUEST_BUFFER,
  SRV_COAP_EXTRACT_MSG1_ERR_UNSUPPORTED_FORWARD_FLOW,
  SRV_COAP_EXTRACT_MSG1_ERR_PREFIX_EXTRACTION,
};

struct srv_coap_extract_message_1_result {
  const enum srv_coap_extract_message_1_status status;
  const struct com_readonly_buffer buffer;
};

/**
 * @brief Extracts the EDHOC Message 1 from the given request buffer, which is
 * expected to contain the raw payload of the CoAP request (including the CBOR
 * prefix).
 * @param[in] request_buffer Input buffer containing the request.
 * @param[in] error_response Metadata of the buffer (pointer and capacity) to
 * write into
 * @return Result struct containing the status code and output field. On
 * success, output contains the parsed message 1 without the CBOR prefix. On
 * failure, output contains the CBOR encoded error payload.
 */
struct srv_coap_extract_message_1_result srv_coap_extract_message_1(
    struct com_readonly_buffer request_buffer,
    struct com_writable_buffer error_response);

coap_pdu_code_t srv_coap_map_extract_message_1_to_pdu_code(
    enum srv_coap_extract_message_1_status status);

bool srv_edhoc_is_message_1(struct com_readonly_buffer request_buffer);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PARSER_H_
