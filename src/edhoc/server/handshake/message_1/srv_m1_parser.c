/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Parsing helpers for EDHOC server Message 1.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/srv_m1_parser.h"

#include <edhoc/common/constants.h>

#include "internal/srv_m1_parser_result_builders.h"

edh_srv_parse_message_1_result_t edh_srv_parse_message_1(
    const com_readonly_buffer_t request_buffer) {
  if (!com_readonly_buffer_is_valid(request_buffer)) {
    return cp_srv_internal_parse_message_1_failure(
        EDH_SRV_MSG1_PARSE_ERR_INVALID_REQUEST_BUFFER);
  }

  if (request_buffer.bytes[0] != EDH_COM_CBOR_TRUE) {
    return cp_srv_internal_parse_message_1_failure(
        EDH_SRV_MSG1_PARSE_ERR_PREFIX_MISSING);
  }
  return cp_srv_internal_parse_message_1_ok((com_readonly_buffer_t){
      .bytes = request_buffer.bytes + 1,
      .length = request_buffer.length - 1,
  });
}

const char* edh_srv_parse_message_1_status_to_string(
    const edh_srv_parse_message_1_status_t status) {
  switch (status) {
  case EDH_SRV_MSG1_PARSE_OK:
    return "ok";
  case EDH_SRV_MSG1_PARSE_ERR_PREFIX_MISSING:
    return "prefix missing";
  case EDH_SRV_MSG1_PARSE_ERR_INVALID_REQUEST_BUFFER:
    return "invalid request buffer";
  default:
    return "unknown";
  }
}
