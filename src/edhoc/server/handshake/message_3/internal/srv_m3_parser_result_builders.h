/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of parsing message 3 EDHOC requests
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_INTERNAL_SRV_M3_PARSER_RESULT_BUILDERS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_INTERNAL_SRV_M3_PARSER_RESULT_BUILDERS_H_

#include "edhoc/server/handshake/message_3/srv_m3_parser.h"

struct edh_srv_parse_message_3_result cp_srv_internal_parse_message_3_failure(
    enum edh_srv_parse_message_3_status status);

struct edh_srv_parse_message_3_result cp_srv_internal_parse_message_3_ok(
    struct com_readonly_buffer parsed_message_3);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_INTERNAL_SRV_M3_PARSER_RESULT_BUILDERS_H_