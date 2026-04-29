/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Parsing helpers for EDHOC server Message 3.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_3/srv_m3_parser.h"

#include <edhoc_helpers.h>

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"

static struct srv_edhoc_parse_message_3_result ok(
    const struct com_readonly_buffer buffer) {
  return (struct srv_edhoc_parse_message_3_result){
      .status = SRV_EDHOC_MSG3_PARSE_OK,
      .buffer = buffer,
  };
}

static struct srv_edhoc_parse_message_3_result failure(
    const enum srv_edhoc_parse_message_3_status status,
    const struct com_readonly_buffer error_buffer) {
  return (struct srv_edhoc_parse_message_3_result){
      .status = status,
      .buffer = error_buffer,
  };
}

static struct srv_edhoc_parse_message_3_result invalid_error_buffer(void) {
  return (struct srv_edhoc_parse_message_3_result){
      .status = SRV_EDHOC_MSG3_PARSE_ERR_INVALID_ERROR_BUFFER,
  };
}

struct srv_edhoc_parse_message_3_result srv_edhoc_parse_message_3(
    const struct com_readonly_buffer request_buffer,
    const struct edhoc_context* edhoc_ctx,
    const struct com_writable_buffer error_response) {
  if (!com_writable_buffer_is_writable(error_response)) {
    return invalid_error_buffer();
  }

  if (!com_readonly_buffer_has_content(request_buffer)) {
    return failure(
        SRV_EDHOC_MSG3_PARSE_ERR_EMPTY_REQUEST_BUFFER,
        com_edhoc_add_internal_error_view(
            "Message 3 Parser error: Empty request buffer", error_response));
  }
  if (edhoc_ctx == NULL) {
    return failure(SRV_EDHOC_MSG3_PARSE_ERR_NULL_EDHOC_CONTEXT,
                   com_edhoc_add_internal_error_view(
                       "Message 3 Parser error: Null context", error_response));
  }

  struct edhoc_extracted_fields extracted_fields =
      (struct edhoc_extracted_fields){
          .buffer = request_buffer.bytes,
          .buffer_size = request_buffer.length,
          .edhoc_message_ptr = request_buffer.bytes,
          .edhoc_message_size = request_buffer.length,
      };
  if (edhoc_extract_connection_id(&extracted_fields) != EDHOC_SUCCESS) {
    return failure(
        SRV_EDHOC_MSG3_PARSE_ERR_CON_ID_EXTRACTION_FAILED,
        com_edhoc_add_internal_error_view(
            "Message 3 Parser error: Connection ID extraction failed",
            error_response));
  }
  if (!edhoc_connection_id_equal(&extracted_fields.extracted_conn_id,
                                 &edhoc_ctx->private_cid)) {
    return failure(SRV_EDHOC_MSG3_PARSE_ERR_UNEXPECTED_CONNECTION_ID,
                   com_edhoc_add_internal_error_view(
                       "Message 3 Parser error: Unexpected connection ID",
                       error_response));
  }
  return ok((struct com_readonly_buffer){
      .bytes = extracted_fields.edhoc_message_ptr,
      .length = extracted_fields.edhoc_message_size,
  });
}

const char* srv_edhoc_parse_message_3_status_to_string(
    const enum srv_edhoc_parse_message_3_status status) {
  switch (status) {
  case SRV_EDHOC_MSG3_PARSE_OK:
    return "ok";
  case SRV_EDHOC_MSG3_PARSE_ERR_EMPTY_REQUEST_BUFFER:
    return "invalid request buffer";
  case SRV_EDHOC_MSG3_PARSE_ERR_NULL_EDHOC_CONTEXT:
    return "null EDHOC context";
  case SRV_EDHOC_MSG3_PARSE_ERR_CON_ID_EXTRACTION_FAILED:
    return "connection ID extraction failed";
  case SRV_EDHOC_MSG3_PARSE_ERR_UNEXPECTED_CONNECTION_ID:
    return "unexpected connection ID";
  default:
    return "unknown";
  }
}
