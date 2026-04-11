/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of parsing message 3 EDHOC requests
 */

#include "edhoc/server/handshake/message_3/internal/srv_m3_parser_result_builders.h"

struct srv_edhoc_parse_message_3_result
srv_coap_internal_parse_message_3_failure(
    const enum srv_edhoc_parse_message_3_status status) {
  return (struct srv_edhoc_parse_message_3_result){
      .status = status,
      .parsed_message_3 = {.bytes = NULL, .length = 0},
  };
}

inline struct srv_edhoc_parse_message_3_result
srv_coap_internal_parse_message_3_ok(
    const struct com_readonly_buffer parsed_message_3) {
  return (struct srv_edhoc_parse_message_3_result){
      .status = SRV_EDHOC_MSG3_PARSE_OK, .parsed_message_3 = parsed_message_3};
}