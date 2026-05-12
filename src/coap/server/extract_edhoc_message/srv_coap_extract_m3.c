/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 12/05/2026
 * @brief Module to extract EDHOC Message 3 from the request payload.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/server/extract_edhoc_message/srv_coap_extract_m3.h"

#include <edhoc_helpers.h>

#include "coap/server/extract_edhoc_message/internal/srv_coap_extract_m3_result_builders.h"
#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"

struct srv_coap_extract_message_3_result srv_coap_extract_message_3(
    const struct com_readonly_buffer request_buffer,
    const struct edhoc_context* edhoc_ctx,
    const struct com_writable_buffer error_response) {
  if (!com_writable_buffer_is_writable(error_response)) {
    return srv_coap_parse_message_3_invalid_error_buffer();
  }
  if (!com_readonly_buffer_has_content(request_buffer)) {
    return srv_coap_parse_message_3_failure(
        SRV_COAP_EXTRACT_MSG3_ERR_EMPTY_REQUEST_BUFFER,
        com_edhoc_add_internal_error_view(
            "Message 3 Extract error: Empty request buffer", error_response));
  }
  if (edhoc_ctx == NULL) {
    return srv_coap_parse_message_3_failure(
        SRV_COAP_EXTRACT_MSG3_ERR_NULL_EDHOC_CONTEXT,
        com_edhoc_add_internal_error_view(
            "Message 3 Extract error: Null context", error_response));
  }

  struct edhoc_extracted_fields extracted_fields = {
      .buffer = request_buffer.bytes,
      .buffer_size = request_buffer.length,
      .edhoc_message_ptr = request_buffer.bytes,
      .edhoc_message_size = request_buffer.length,
  };
  if (edhoc_extract_connection_id(&extracted_fields) != EDHOC_SUCCESS) {
    return srv_coap_parse_message_3_failure(
        SRV_COAP_EXTRACT_MSG3_ERR_CON_ID_EXTRACTION_FAILED,
        com_edhoc_add_internal_error_view(
            "Message 3 Extract error: Connection ID extraction failed",
            error_response));
  }
  if (!edhoc_connection_id_equal(&extracted_fields.extracted_conn_id,
                                 &edhoc_ctx->private_cid)) {
    return srv_coap_parse_message_3_failure(
        SRV_COAP_EXTRACT_MSG3_ERR_UNEXPECTED_CONNECTION_ID,
        com_edhoc_add_internal_error_view(
            "Message 3 Extract error: Unexpected connection ID",
            error_response));
  }
  return srv_coap_parse_message_3_ok((struct com_readonly_buffer){
      .bytes = extracted_fields.edhoc_message_ptr,
      .length = extracted_fields.edhoc_message_size,
  });
}

coap_pdu_code_t srv_coap_map_extract_message_3_to_pdu_code(
    const enum srv_coap_extract_message_3_status status) {
  switch (status) {
  case SRV_COAP_EXTRACT_MSG3_OK:
    return COAP_RESPONSE_CODE_CHANGED;
  case SRV_COAP_EXTRACT_MSG3_ERR_EMPTY_REQUEST_BUFFER:
  case SRV_COAP_EXTRACT_MSG3_ERR_CON_ID_EXTRACTION_FAILED:
  case SRV_COAP_EXTRACT_MSG3_ERR_UNEXPECTED_CONNECTION_ID:
    return COAP_RESPONSE_CODE_BAD_REQUEST;
  case SRV_COAP_EXTRACT_MSG3_ERR_INVALID_ERROR_BUFFER:
  case SRV_COAP_EXTRACT_MSG3_ERR_NULL_EDHOC_CONTEXT:
  default:
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
}

bool srv_edhoc_is_message_3(const struct com_readonly_buffer request_buffer,
                            const struct edhoc_context* context) {
  if (!com_readonly_buffer_has_content(request_buffer) || context == NULL) {
    return false;
  }

  struct edhoc_extracted_fields extracted_fields = {
      .buffer = request_buffer.bytes,
      .buffer_size = request_buffer.length,
  };
  if (edhoc_extract_connection_id(&extracted_fields) != EDHOC_SUCCESS) {
    return false;
  }
  return edhoc_connection_id_equal(&extracted_fields.extracted_conn_id,
                                   &context->private_cid);
}

