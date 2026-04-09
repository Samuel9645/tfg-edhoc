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

enum edh_srv_parse_message_3_status {
  EDH_SRV_MSG3_PARSE_OK = 0,
  EDH_SRV_MSG3_PARSE_ERR_INVALID_REQUEST_BUFFER,
  EDH_SRV_MSG3_PARSE_ERR_NULL_EDHOC_CONTEXT,
  EDH_SRV_MSG3_PARSE_ERR_CON_ID_EXTRACTION_FAILED,
  EDH_SRV_MSG3_PARSE_ERR_UNEXPECTED_CONNECTION_ID,
};

struct edh_srv_parse_message_3_result {
  enum edh_srv_parse_message_3_status status;
  struct com_readonly_buffer parsed_message_3;
};

/**
 * @brief Parse and validate EDHOC Message 3 for the active session.
 * @param[in] request_buffer Read-only buffer containing the raw Message 3
 * payload, including the prepended connection ID.
 * @param[in] edhoc_ctx Pointer to the edhoc context to use the helpers to
 * extract the connection ID and verify it against the expected value.
 * @return Result struct containing the parsing status and a view over the
 * parsed Message 3 payload (excluding the connection ID) on success, or an
 * empty view with error code on failure.
 */
struct edh_srv_parse_message_3_result edh_srv_parse_message_3(
    struct com_readonly_buffer request_buffer,
    const struct edhoc_context* edhoc_ctx);

const char* edh_srv_parse_message_3_status_to_string(
    enum edh_srv_parse_message_3_status status);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_PARSER_H_
