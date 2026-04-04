/**
 * @file parser.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Parsing helpers for EDHOC server Message 1.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_PARSER_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_PARSER_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "common/data_models.h"
#include "edhoc/server/handshake/message_1/result.h"

edh_srv_message_1_handler_status_t edh_srv_remove_cbor_true_prefix(
    const uint8_t** payload, size_t* length);

/**
 * @brief Parse EDHOC Message 1 payload into a clean payload view for the
 * handler.
 *
 * Validates Message 1 framing and strips the CBOR TRUE prefix. On success,
 * `parsed_payload` points to the original input buffer advanced past the
 * prefix, with the corresponding reduced length.
 *
 * @param[in] request_payload Raw incoming payload bytes.
 * @param[in] request_len Raw incoming payload length.
 * @param[out] parsed_payload Output borrowed view without CBOR TRUE prefix.
 * @return true if payload is valid Message 1 and output is populated;
 * false otherwise.
 */
bool edh_srv_parse_message_1(const uint8_t* request_payload, size_t request_len,
                             com_readonly_buffer_t* parsed_payload);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_PARSER_H_
