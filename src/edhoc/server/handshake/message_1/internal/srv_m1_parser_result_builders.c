/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of parsing message 1 EDHOC requests
 */

#include "srv_m1_parser_result_builders.h"

struct edh_srv_parse_message_1_result cp_srv_internal_parse_message_1_failure(
    enum edh_srv_parse_message_1_status status) {
  return (struct edh_srv_parse_message_1_result){
      .status = status,
      .parsed_message_1 = {.bytes = NULL, .length = 0},
  };
}

inline struct edh_srv_parse_message_1_result cp_srv_internal_parse_message_1_ok(
    const struct com_readonly_buffer parsed_message_1) {
  return (struct edh_srv_parse_message_1_result){
      .status = EDH_SRV_MSG1_PARSE_OK, .parsed_message_1 = parsed_message_1};
}