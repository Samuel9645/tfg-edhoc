/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Message 3 processing on the responder side.
 * @see [RFC
 * 9528 5.4.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-processing-of-mess)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_3/srv_m3_process.h"

#include <edhoc.h>

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

static struct srv_edhoc_message_3_process_result ok(void) {
  return (struct srv_edhoc_message_3_process_result){
      .status = SRV_EDHOC_MSG3_PROCESS_OK,
  };
}

static struct srv_edhoc_message_3_process_result failure(
    const enum srv_edhoc_message_3_process_status status,
    const struct com_readonly_buffer error_buffer) {
  return (struct srv_edhoc_message_3_process_result){
      .status = status,
      .error_buffer = error_buffer,
  };
}

static struct srv_edhoc_message_3_process_result invalid_error_buffer(void) {
  return (struct srv_edhoc_message_3_process_result){
      .status = SRV_EDHOC_MSG3_PROCESS_ERR_INVALID_ERROR_BUFFER,
  };
}

static struct com_readonly_buffer add_processing_error_to_buffer(
    const struct edhoc_context* context, struct com_writable_buffer* buffer) {
  return com_edhoc_add_protocol_error_with_description_view(
      context, "Message 3 processing failed", buffer);
}

struct srv_edhoc_message_3_process_result srv_edhoc_process_message_3(
    const struct srv_edhoc_message_3_request request,
    struct com_writable_buffer* error_buffer) {
  if (!com_writable_buffer_is_writable(error_buffer)) {
    return invalid_error_buffer();
  }
  if (request.edhoc_context == NULL) {
    return failure(
        SRV_EDHOC_MSG3_PROCESS_ERR_NULL_EDHOC_CONTEXT,
        com_edhoc_add_internal_error_view("Null EDHOC context", error_buffer));
  }
  if (!com_readonly_buffer_has_content(request.parsed_message_3)) {
    return failure(SRV_EDHOC_MSG3_PROCESS_ERR_EMPTY_PARSED_MESSAGE_3,
                   com_edhoc_add_internal_error_view("Empty parsed message 3",
                                                     error_buffer));
  }

  if (edhoc_message_3_process(
          request.edhoc_context, request.parsed_message_3.bytes,
          request.parsed_message_3.length) != EDHOC_SUCCESS) {
    return failure(
        SRV_EDHOC_MSG3_PROCESS_ERR_EDHOC_MESSAGE_3_PROCESS_FAILED,
        add_processing_error_to_buffer(request.edhoc_context, error_buffer));
  }
  return ok();
}
