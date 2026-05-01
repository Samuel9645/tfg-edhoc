/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Parsing helpers for EDHOC server Message 1.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/srv_m1_parser.h"

#include <edhoc_helpers.h>

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"

static struct srv_edhoc_parse_message_1_result ok(
    const struct com_readonly_buffer buffer) {
  return (struct srv_edhoc_parse_message_1_result){
      .status = SRV_EDHOC_MSG1_PARSE_OK,
      .buffer = buffer,
  };
}

static struct srv_edhoc_parse_message_1_result failure(
    const enum srv_edhoc_parse_message_1_status status,
    const struct com_readonly_buffer error_buffer) {
  return (struct srv_edhoc_parse_message_1_result){
      .status = status,
      .buffer = error_buffer,
  };
}

static struct srv_edhoc_parse_message_1_result invalid_error_buffer(void) {
  return (struct srv_edhoc_parse_message_1_result){
      .status = SRV_EDHOC_MSG1_PARSE_ERR_INVALID_ERROR_BUFFER,
  };
}

struct srv_edhoc_parse_message_1_result srv_edhoc_parse_message_1(
    const struct com_readonly_buffer request_buffer,
    const struct com_writable_buffer error_response) {
  if (!com_writable_buffer_is_writable(error_response)) {
    return invalid_error_buffer();
  }

  if (!com_readonly_buffer_has_content(request_buffer)) {
    return failure(
        SRV_EDHOC_MSG1_PARSE_ERR_EMPTY_REQUEST_BUFFER,
        com_edhoc_add_internal_error_view(
            "Message 1 Parser error: Empty request buffer", error_response));
  }

  struct edhoc_extracted_fields extracted_fields = {
      .buffer = request_buffer.bytes,
      .buffer_size = request_buffer.length,
      .edhoc_message_ptr = request_buffer.bytes,
      .edhoc_message_size = request_buffer.length,
  };
  if (edhoc_extract_flow_info(&extracted_fields) != EDHOC_SUCCESS) {
    return failure(SRV_EDHOC_MSG1_PARSE_ERR_PREFIX_EXTRACTION,
                   com_edhoc_add_internal_error_view(
                       "Message 1 Parser error: Prefix extraction failed",
                       error_response));
  }
  return ok((struct com_readonly_buffer){
      .bytes = extracted_fields.edhoc_message_ptr,
      .length = extracted_fields.edhoc_message_size,
  });
}

bool srv_edhoc_is_message_1(const struct com_readonly_buffer request_buffer) {
  if (!com_readonly_buffer_has_content(request_buffer)) {
    return false;
  }

  struct edhoc_extracted_fields extracted_fields = {
      .buffer = request_buffer.bytes,
      .buffer_size = request_buffer.length,
  };
  return edhoc_extract_flow_info(&extracted_fields) == EDHOC_SUCCESS;
}