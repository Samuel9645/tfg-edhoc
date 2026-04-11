/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Definition for the Message 1 handler
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/srv_m1_handler.h"

#include <edhoc.h>
#include <stdlib.h>

#include "../../../../../include/edhoc/common/add_error/edhoc_error.h"
#include "edhoc/common/com_edhoc_setup.h"
#include "edhoc/server/handshake/message_1/internal/srv_m1_handler_result_builders.h"
#include "edhoc/server/handshake/message_1/srv_m1_errors.h"

/**
 * WHY ARE WE CASTING TO VOID THE RETURN?
 *
 * Protocol errors are more important than internal errors, so in case something
 * bad happens, we always want to report the greater failure.
 */
struct srv_edhoc_message_1_handler_result srv_edhoc_handle_message_1(
    const struct srv_edhoc_message_1_request request,
    struct com_writable_buffer* response) {
  if (!com_writable_buffer_is_writable(response)) {
    return srv_edhoc_message_1_handler_failure(
        SRV_EDHOC_MSG1_HDL_ERR_INVALID_RESPONSE_BUFFER);
  }
  if (request.credentials == NULL) {
    (void)com_edhoc_add_internal_error_to_response("Null credentials",
                                                   response);
    return srv_edhoc_message_1_handler_failure(
        SRV_EDHOC_MSG1_HDL_ERR_NULL_CREDENTIALS);
  }
  if (!com_readonly_buffer_is_valid(request.payload)) {
    (void)com_edhoc_add_internal_error_to_response("Invalid request buffer",
                                                   response);
    return srv_edhoc_message_1_handler_failure(
        SRV_EDHOC_MSG1_HDL_ERR_INVALID_REQUEST_BUFFER);
  }

  struct edhoc_context* edhoc_ctx = calloc(1, sizeof(struct edhoc_context));
  if (!edhoc_ctx) {
    (void)com_edhoc_add_internal_error_to_response("Context calloc failed",
                                                   response);
    return srv_edhoc_message_1_handler_failure(
        SRV_EDHOC_MSG1_HDL_ERR_CALLOC_FAILED);
  }
  int edhoc_api_result =
      com_edhoc_setup_context(edhoc_ctx, request.credentials);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    (void)com_edhoc_add_internal_error_to_response("Context setup failed",
                                                   response);
    free(edhoc_ctx);
    return srv_edhoc_message_1_handler_failure(
        SRV_EDHOC_MSG1_HDL_ERR_EDHOC_CONTEXT_SETUP_FAILED);
  }
  edhoc_api_result = edhoc_message_1_process(edhoc_ctx, request.payload.bytes,
                                             request.payload.length);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    srv_edhoc_message_1_handler_add_error(
        edhoc_ctx, "Message 1 processing failed", response);
    free(edhoc_ctx);
    return srv_edhoc_message_1_handler_failure(
        SRV_EDHOC_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED);
  }
  edhoc_api_result = edhoc_message_2_compose(
      edhoc_ctx, response->bytes, response->capacity, &response->length);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    srv_edhoc_message_1_handler_add_error(
        edhoc_ctx, "Message 2 composing failed", response);
    free(edhoc_ctx);
    return srv_edhoc_message_1_handler_failure(
        SRV_EDHOC_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED);
  }
  if (!com_writable_buffer_has_content(response)) {
    (void)com_edhoc_add_internal_error_to_response(
        "Message 2 compose produced empty buffer", response);
    free(edhoc_ctx);
    return srv_edhoc_message_1_handler_failure(
        SRV_EDHOC_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_EMPTY);
  }
  return srv_edhoc_message_1_handler_ok(edhoc_ctx);
}

const char* srv_edhoc_handle_message_1_status_code_to_string(
    const enum srv_edhoc_message_1_handler_status status) {
  switch (status) {
  case SRV_EDHOC_MSG1_HDL_OK:
    return "ok";
  case SRV_EDHOC_MSG1_HDL_ERR_NULL_CREDENTIALS:
    return "null credentials";
  case SRV_EDHOC_MSG1_HDL_ERR_INVALID_REQUEST_BUFFER:
    return "invalid request buffer";
  case SRV_EDHOC_MSG1_HDL_ERR_INVALID_RESPONSE_BUFFER:
    return "invalid response buffer";
  case SRV_EDHOC_MSG1_HDL_ERR_CALLOC_FAILED:
    return "calloc failed";
  case SRV_EDHOC_MSG1_HDL_ERR_EDHOC_CONTEXT_SETUP_FAILED:
    return "EDHOC context setup failed";
  case SRV_EDHOC_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED:
    return "EDHOC message 1 process failed";
  case SRV_EDHOC_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED:
    return "EDHOC message 2 compose failed";
  default:
    return "unknown";
  }
}