/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of parsing message 1 EDHOC requests
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_INTERNAL_SRV_M1_PARSER_RESULT_BUILDERS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_INTERNAL_SRV_M1_PARSER_RESULT_BUILDERS_H_

#include "edhoc/server/handshake/message_1/srv_m1_parser.h"

struct srv_edhoc_parse_message_1_result
srv_coap_internal_parse_message_1_failure(
    enum srv_edhoc_parse_message_1_status status);

struct srv_edhoc_parse_message_1_result srv_coap_internal_parse_message_1_ok(
    struct com_readonly_buffer parsed_message_1);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_INTERNAL_SRV_M1_PARSER_RESULT_BUILDERS_H_