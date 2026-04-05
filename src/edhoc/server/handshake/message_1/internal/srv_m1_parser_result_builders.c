/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of parsing message 1 EDHOC requests
 */

#include "srv_m1_parser_result_builders.h"

edh_srv_parse_message_1_result_t cp_srv_internal_parse_message_1_failure(
    edh_srv_parse_message_1_status_t status) {
  return (edh_srv_parse_message_1_result_t){
      .status = status,
      .parsed_message_1 = {.bytes = NULL, .length = 0},
  };
}

inline edh_srv_parse_message_1_result_t cp_srv_internal_parse_message_1_ok(
    const com_readonly_buffer_t parsed_message_1) {
  return (edh_srv_parse_message_1_result_t){
      .status = EDH_SRV_MSG1_PARSE_OK, .parsed_message_1 = parsed_message_1};
}