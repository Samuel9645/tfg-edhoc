/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Parsing helpers for EDHOC server Message 3.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_3/srv_m3_parser.h"

#include <edhoc.h>
#include <edhoc_helpers.h>
#include <edhoc_values.h>

#include "internal/srv_m3_parser_result_builders.h"

edh_srv_parse_message_3_result_t edh_srv_parse_message_3(
    const com_readonly_buffer_t request_buffer,
    const struct edhoc_context* edhoc_ctx) {
  if (!com_readonly_buffer_is_valid(request_buffer)) {
    return cp_srv_internal_parse_message_3_failure(
        EDH_SRV_MSG3_PARSE_ERR_INVALID_REQUEST_BUFFER);
  }
  if (edhoc_ctx == NULL) {
    return cp_srv_internal_parse_message_3_failure(
        EDH_SRV_MSG3_PARSE_ERR_NULL_EDHOC_CONTEXT);
  }

  struct edhoc_extracted_fields extracted_fields =
      (struct edhoc_extracted_fields){
          .buffer = request_buffer.bytes,
          .buffer_size = request_buffer.length,
          .edhoc_message_ptr = request_buffer.bytes,
          .edhoc_message_size = request_buffer.length,
      };
  if (edhoc_extract_connection_id(&extracted_fields) != EDHOC_SUCCESS) {
    return cp_srv_internal_parse_message_3_failure(
        EDH_SRV_MSG3_PARSE_ERR_CON_ID_EXTRACTION_FAILED);
  }
  if (edhoc_connection_id_equal(&extracted_fields.extracted_conn_id,
                                &edhoc_ctx->private_cid)) {
    return cp_srv_internal_parse_message_3_ok((struct com_readonly_buffer){
        .bytes = extracted_fields.edhoc_message_ptr,
        .length = extracted_fields.edhoc_message_size,
    });
  }
  return cp_srv_internal_parse_message_3_failure(
      EDH_SRV_MSG3_PARSE_ERR_UNEXPECTED_CONNECTION_ID);
}

const char* edh_srv_parse_message_3_status_to_string(
    const edh_srv_parse_message_3_status_t status) {
  switch (status) {
  case EDH_SRV_MSG3_PARSE_OK:
    return "ok";
  case EDH_SRV_MSG3_PARSE_ERR_INVALID_REQUEST_BUFFER:
    return "invalid request buffer";
  case EDH_SRV_MSG3_PARSE_ERR_NULL_EDHOC_CONTEXT:
    return "null EDHOC context";
  case EDH_SRV_MSG3_PARSE_ERR_CON_ID_EXTRACTION_FAILED:
    return "connection ID extraction failed";
  case EDH_SRV_MSG3_PARSE_ERR_UNEXPECTED_CONNECTION_ID:
    return "unexpected connection ID";
  default:
    return "unknown";
  }
}
