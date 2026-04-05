/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of parsing message 3 EDHOC requests
 */

#include "srv_m3_parser_result_builders.h"

edh_srv_parse_message_3_result_t cp_srv_internal_parse_message_3_failure(
    const edh_srv_parse_message_3_status_t status) {
  return (edh_srv_parse_message_3_result_t){
      .status = status,
      .parsed_message_3 = {.bytes = NULL, .length = 0},
  };
}

inline edh_srv_parse_message_3_result_t cp_srv_internal_parse_message_3_ok(
    const com_readonly_buffer_t parsed_message_3) {
  return (edh_srv_parse_message_3_result_t){
      .status = EDH_SRV_MSG3_PARSE_OK, .parsed_message_3 = parsed_message_3};
}