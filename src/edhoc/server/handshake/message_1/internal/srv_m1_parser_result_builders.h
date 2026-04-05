/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of parsing message 1 EDHOC requests
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_INTERNAL_SRV_M1_PARSER_RESULT_BUILDERS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_INTERNAL_SRV_M1_PARSER_RESULT_BUILDERS_H_

#include "edhoc/server/handshake/message_1/srv_m1_parser.h"

edh_srv_parse_message_1_result_t cp_srv_internal_parse_message_1_failure(
    edh_srv_parse_message_1_status_t status);

edh_srv_parse_message_1_result_t cp_srv_internal_parse_message_1_ok(
    com_readonly_buffer_t parsed_message_1);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_INTERNAL_SRV_M1_PARSER_RESULT_BUILDERS_H_