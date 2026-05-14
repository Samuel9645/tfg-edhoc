/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Module to extract the Message 1 from the request with the CBOR true
 * prefix
 *  * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [RFC 9528
 * A.2](https://datatracker.ietf.org/doc/html/rfc9528#name-transferring-edhoc-over-coa)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/server/extract_edhoc_message/srv_coap_extract_m1.h"

#include <edhoc_helpers.h>

#include "coap/server/extract_edhoc_message/internal/srv_coap_extract_m1_result_builders.h"
#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"

struct srv_coap_extract_message_1_result srv_coap_extract_message_1(
    const struct com_readonly_buffer request_buffer,
    const struct com_writable_buffer error_response) {
  if (!com_writable_buffer_is_writable(error_response)) {
    return srv_coap_parse_message_1_invalid_error_buffer();
  }

  if (!com_readonly_buffer_has_content(request_buffer)) {
    return srv_coap_parse_message_1_failure(
        SRV_COAP_EXTRACT_MSG1_ERR_EMPTY_REQUEST_BUFFER,
        com_edhoc_add_internal_error_view(
            "Message 1 Extract error: Empty request buffer", error_response));
  }

  struct edhoc_extracted_fields extracted_fields = {
      .buffer = request_buffer.bytes,
      .buffer_size = request_buffer.length,
      .edhoc_message_ptr = request_buffer.bytes,
      .edhoc_message_size = request_buffer.length,
  };
  if (edhoc_extract_flow_info(&extracted_fields) != EDHOC_SUCCESS) {
    return srv_coap_parse_message_1_failure(
        SRV_COAP_EXTRACT_MSG1_ERR_PREFIX_EXTRACTION,
        com_edhoc_add_internal_error_view(
            "Message 1 Extract error: Prefix extraction failed",
            error_response));
  }
  if (!extracted_fields.is_forward_flow) {
    return srv_coap_parse_message_1_failure(
        SRV_COAP_EXTRACT_MSG1_ERR_UNSUPPORTED_FORWARD_FLOW,
        com_edhoc_add_internal_error_view(
            "Message 1 Extract error: Unsupported forward flow",
            error_response));
  }
  return srv_coap_parse_message_1_ok((struct com_readonly_buffer){
      .bytes = extracted_fields.edhoc_message_ptr,
      .length = extracted_fields.edhoc_message_size,
  });
}

coap_pdu_code_t srv_coap_map_extract_message_1_to_pdu_code(
    const enum srv_coap_extract_message_1_status status) {
  switch (status) {
  case SRV_COAP_EXTRACT_MSG1_OK:
    return COAP_RESPONSE_CODE_CHANGED;
  case SRV_COAP_EXTRACT_MSG1_ERR_EMPTY_REQUEST_BUFFER:
  case SRV_COAP_EXTRACT_MSG1_ERR_UNSUPPORTED_FORWARD_FLOW:
  case SRV_COAP_EXTRACT_MSG1_ERR_PREFIX_EXTRACTION:
    return COAP_RESPONSE_CODE_BAD_REQUEST;
  case SRV_COAP_EXTRACT_MSG1_ERR_INVALID_ERROR_BUFFER:
  default:
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }
}

bool srv_edhoc_is_message_1(const struct com_readonly_buffer request_buffer) {
  if (!com_readonly_buffer_has_content(request_buffer)) {
    return false;
  }

  struct edhoc_extracted_fields extracted_fields = {
      .buffer = request_buffer.bytes,
      .buffer_size = request_buffer.length,
  };
  if (edhoc_extract_flow_info(&extracted_fields) != EDHOC_SUCCESS) {
    return false;
  }
  return extracted_fields.is_forward_flow;
}