/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Parsing helpers for EDHOC server Message 1.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PARSER_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PARSER_H_

#include "common/com_data_models.h"

enum srv_edhoc_parse_message_1_status {
  SRV_EDHOC_MSG1_PARSE_OK = 0,
  SRV_EDHOC_MSG1_PARSE_ERR_INVALID_ERROR_BUFFER,
  SRV_EDHOC_MSG1_PARSE_ERR_EMPTY_REQUEST_BUFFER,
  SRV_EDHOC_MSG1_PARSE_ERR_UNSUPPORTED_FORWARD_FLOW,
  SRV_EDHOC_MSG1_PARSE_ERR_PREFIX_EXTRACTION,
};

struct srv_edhoc_parse_message_1_result {
  const enum srv_edhoc_parse_message_1_status status;
  const struct com_readonly_buffer buffer;
};

/**
 * @brief Parse EDHOC Message 1 payload into a clean payload view for the
 * handler.
 * @param[in] request_buffer Input buffer containing the request.
 * @param[in] error_response Metadata of the buffer (pointer and capacity) to
 * write into
 * @return Result struct containing the status code and output field. On
 * success, output contains the parsed message 1 without the CBOR prefix. On
 * failure, output contains the CBOR encoded error payload.
 */
struct srv_edhoc_parse_message_1_result srv_edhoc_parse_message_1(
    struct com_readonly_buffer request_buffer,
    struct com_writable_buffer error_response);

bool srv_edhoc_is_message_1(struct com_readonly_buffer request_buffer);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PARSER_H_
