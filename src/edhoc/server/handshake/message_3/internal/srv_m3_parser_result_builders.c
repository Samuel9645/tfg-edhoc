/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of parsing message 3 EDHOC requests
 */

#include "srv_m3_parser_result_builders.h"

struct edh_srv_parse_message_3_result cp_srv_internal_parse_message_3_failure(
    const enum edh_srv_parse_message_3_status status) {
  return (struct edh_srv_parse_message_3_result){
      .status = status,
      .parsed_message_3 = {.bytes = NULL, .length = 0},
  };
}

inline struct edh_srv_parse_message_3_result cp_srv_internal_parse_message_3_ok(
    const struct com_readonly_buffer parsed_message_3) {
  return (struct edh_srv_parse_message_3_result){
      .status = EDH_SRV_MSG3_PARSE_OK, .parsed_message_3 = parsed_message_3};
}