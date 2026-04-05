/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Parsing helpers for EDHOC server Message 1.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PARSER_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PARSER_H_

#include "common/data_models.h"

typedef enum edh_srv_parse_message_1_status {
  EDH_SRV_MSG1_PARSE_OK = 0,
  EDH_SRV_MSG1_PARSE_ERR_INVALID_REQUEST_BUFFER,
  EDH_SRV_MSG1_PARSE_ERR_PREFIX_MISSING
} edh_srv_parse_message_1_status_t;

typedef struct edh_srv_parse_message_1_result {
  edh_srv_parse_message_1_status_t status;
  com_readonly_buffer_t parsed_message_1;
} edh_srv_parse_message_1_result_t;

/**
 * @brief Parse EDHOC Message 1 payload into a clean payload view for the
 * handler.
 * @param[in] request_buffer Input buffer containing the request.
 * @return Result struct containing the status code and the parsed message 1
 * without the CBOR prefix on success, empty parse result with the corresponding
 * error status on failure.
 */
edh_srv_parse_message_1_result_t edh_srv_parse_message_1(
    com_readonly_buffer_t request_buffer);

/**
 * @brief Convert a parsing status code into a human-readable string.
 * @param status status of the parsing operation
 * @return human-readable string describing the parsing status, useful for
 * logging and debugging.
 */
const char* edh_srv_parse_message_1_status_to_string(
    edh_srv_parse_message_1_status_t status);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PARSER_H_
