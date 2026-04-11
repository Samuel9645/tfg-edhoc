/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Definition for the Message 3 handler.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_3/srv_m3_handler.h"

#include <edhoc.h>

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

/**
 * WHY ARE WE CASTING TO VOID THE RETURN?
 *
 * Protocol errors are more important than internal errors, so in case something
 * bad happens, we always want to report the greater failure.
 */
enum srv_edhoc_message_3_handler_status srv_edhoc_handle_message_3(
    const struct srv_edhoc_message_3_request request,
    struct com_writable_buffer* response_buffer) {
  if (!com_writable_buffer_is_writable(response_buffer)) {
    return EDH_MSG3_HDL_ERR_INVALID_RESPONSE_BUFFER;
  }

  if (request.edhoc_context == NULL) {
    (void)com_edhoc_add_internal_error_to_response("Null EDHOC context",
                                                   response_buffer);
    return EDH_MSG3_HDL_ERR_NULL_EDHOC_CONTEXT;
  }
  if (!com_readonly_buffer_is_valid(request.parsed_message_3)) {
    (void)com_edhoc_add_internal_error_to_response(
        "Invalid EDHOC parsed message 3", response_buffer);
    return EDH_MSG3_HDL_ERR_INVALID_PARSED_MESSAGE_3;
  }

  int edhoc_api_result = edhoc_message_3_process(
      request.edhoc_context, request.parsed_message_3.bytes,
      request.parsed_message_3.length);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    (void)com_edhoc_add_edhoc_error_to_response_with_description(
        request.edhoc_context, "Message 3 processing failed", response_buffer);
    return EDH_MSG3_HDL_ERR_MESSAGE_3_PROCESS_FAILED;
  }
  edhoc_api_result = edhoc_message_4_compose(
      request.edhoc_context, response_buffer->bytes, response_buffer->capacity,
      &response_buffer->length);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    (void)com_edhoc_add_edhoc_error_to_response_with_description(
        request.edhoc_context, "Message 4 composing failed", response_buffer);
    return EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_FAILED;
  }
  if (!com_writable_buffer_has_content(response_buffer)) {
    (void)com_edhoc_add_internal_error_to_response(
        "Message 4 compose produced empty buffer", response_buffer);
    return EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_EMPTY;
  }
  return EDH_MSG3_HDL_OK;
}

const char* srv_edhoc_handle_message_3_status_code_to_string(
    const enum srv_edhoc_message_3_handler_status status) {
  switch (status) {
  case EDH_MSG3_HDL_OK:
    return "ok";
  case EDH_MSG3_HDL_ERR_NULL_EDHOC_CONTEXT:
    return "null edhoc context";
  case EDH_MSG3_HDL_ERR_INVALID_PARSED_MESSAGE_3:
    return "invalid parsed message 3";
  case EDH_MSG3_HDL_ERR_INVALID_RESPONSE_BUFFER:
    return "invalid response buffer";
  case EDH_MSG3_HDL_ERR_MESSAGE_3_PROCESS_FAILED:
    return "message 3 process failed";
  case EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_FAILED:
    return "message 4 compose failed";
  default:
    return "unknown";
  }
}
