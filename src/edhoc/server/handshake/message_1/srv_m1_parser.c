/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Parsing helpers for EDHOC server Message 1.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/srv_m1_parser.h"

#include <edhoc_helpers.h>

#include "edhoc/server/handshake/message_1/internal/srv_m1_parser_result_builders.h"

struct edh_srv_parse_message_1_result edh_srv_parse_message_1(
    const struct com_readonly_buffer request_buffer) {
  if (!com_readonly_buffer_is_valid(request_buffer)) {
    return cp_srv_internal_parse_message_1_failure(
        EDH_SRV_MSG1_PARSE_ERR_INVALID_REQUEST_BUFFER);
  }

  struct edhoc_extracted_fields extracted_fields = {
      .buffer = request_buffer.bytes,
      .buffer_size = request_buffer.length,
      .edhoc_message_ptr = request_buffer.bytes,
      .edhoc_message_size = request_buffer.length,
  };
  if (edhoc_extract_flow_info(&extracted_fields) != EDHOC_SUCCESS) {
    return cp_srv_internal_parse_message_1_failure(
        EDH_SRV_MSG1_PARSE_ERR_PREFIX_EXTRACTION);
  }
  return cp_srv_internal_parse_message_1_ok((struct com_readonly_buffer){
      .bytes = extracted_fields.edhoc_message_ptr,
      .length = extracted_fields.edhoc_message_size,
  });
}

const char* edh_srv_parse_message_1_status_to_string(
    const enum edh_srv_parse_message_1_status status) {
  switch (status) {
  case EDH_SRV_MSG1_PARSE_OK:
    return "ok";
  case EDH_SRV_MSG1_PARSE_ERR_PREFIX_EXTRACTION:
    return "prefix missing";
  case EDH_SRV_MSG1_PARSE_ERR_INVALID_REQUEST_BUFFER:
    return "invalid request buffer";
  default:
    return "unknown";
  }
}
