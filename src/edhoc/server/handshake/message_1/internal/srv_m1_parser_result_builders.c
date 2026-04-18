/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of parsing message 1 EDHOC requests
 */

#include "edhoc/server/handshake/message_1/internal/srv_m1_parser_result_builders.h"

struct srv_edhoc_parse_message_1_result
srv_coap_internal_parse_message_1_failure(
    const enum srv_edhoc_parse_message_1_status status,
    const struct com_readonly_buffer error_buffer) {
  return (struct srv_edhoc_parse_message_1_result){
      .status = status,
      .buffer = error_buffer,
  };
}

struct srv_edhoc_parse_message_1_result srv_coap_internal_parse_message_1_ok(
    const struct com_readonly_buffer parsed_message_1) {
  return (struct srv_edhoc_parse_message_1_result){
      .status = SRV_EDHOC_MSG1_PARSE_OK,
      .buffer = parsed_message_1,
  };
}

struct srv_edhoc_parse_message_1_result
srv_coap_internal_parse_message_1_invalid_response_buffer_failure(void) {
  return (struct srv_edhoc_parse_message_1_result){
      .status = SRV_EDHOC_MSG1_PARSE_ERR_INVALID_REQUEST_BUFFER};
}