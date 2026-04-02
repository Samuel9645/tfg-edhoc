/**
 * @file parser.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Parsing helpers for EDHOC server Message 3.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_PARSER_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_PARSER_H_

#include <edhoc_helpers.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Parse and validate EDHOC Message 3 for the active session.
 *
 * Populates `extracted_fields` with a view over the input payload, extracts
 * the peer connection ID, and verifies it matches `edhoc_ctx->private_cid`.
 * This function performs parsing/validation only and does not mutate protocol
 * state.
 *
 * @param[in] request_payload Raw incoming payload bytes.
 * @param[in] request_len Raw incoming payload length.
 * @param[in] edhoc_ctx Active session context used for CID match validation.
 * @param[out] extracted_fields Output parsed/extracted fields view.
 * @return true if payload is valid Message 3 for this session; false
 * otherwise.
 */
bool edh_srv_parse_message_3(const uint8_t* request_payload, size_t request_len,
                             const struct edhoc_context* edhoc_ctx,
                             struct edhoc_extracted_fields* extracted_fields);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_PARSER_H_
