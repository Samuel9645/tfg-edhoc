/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Parsing helpers for EDHOC server Message 3.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_PARSER_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_PARSER_H_

#include <edhoc.h>

#include "common/com_data_models.h"

enum srv_edhoc_parse_message_3_status {
  SRV_EDHOC_MSG3_PARSE_OK = 0,
  SRV_EDHOC_MSG3_PARSE_ERR_INVALID_ERROR_BUFFER,
  SRV_EDHOC_MSG3_PARSE_ERR_EMPTY_REQUEST_BUFFER,
  SRV_EDHOC_MSG3_PARSE_ERR_NULL_EDHOC_CONTEXT,
  SRV_EDHOC_MSG3_PARSE_ERR_CON_ID_EXTRACTION_FAILED,
  SRV_EDHOC_MSG3_PARSE_ERR_UNEXPECTED_CONNECTION_ID,
};

struct srv_edhoc_parse_message_3_result {
  const enum srv_edhoc_parse_message_3_status status;
  const struct com_readonly_buffer buffer;
};

/**
 * @brief Parse and validate EDHOC Message 3 for the active session.
 * @param[in] request_buffer Read-only buffer containing the raw Message 3
 * payload, including the prepended connection ID.
 * @param[in] edhoc_ctx Pointer to the edhoc context to use the helpers to
 * extract the connection ID and verify it against the expected value.
 * @param[in] error_response Metadata of the buffer (pointer and capacity) to
 * write into
 * @return Result struct containing the parsing status and a view over the
 * parsed Message 3 payload (excluding the connection ID) on success, or an
 * empty view with error code on failure. On failure, error_response is filled
 * with CBOR encoded error payload.
 */
struct srv_edhoc_parse_message_3_result srv_edhoc_parse_message_3(
    struct com_readonly_buffer request_buffer,
    const struct edhoc_context* edhoc_ctx,
    struct com_writable_buffer error_response);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_PARSER_H_
