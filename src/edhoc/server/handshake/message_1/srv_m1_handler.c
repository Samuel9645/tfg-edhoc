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

#include "edhoc/common/setup.h"
#include "edhoc/server/handshake/message_1/internal/srv_m1_handler_result_builders.h"
#include "edhoc/server/handshake/message_1/srv_m1_errors.h"

ehd_srv_message_1_handler_result_t edh_srv_handle_message_1(
    const edh_srv_message_1_request_t request,
    com_writable_buffer_t* response) {
  if (request.credentials == NULL) {
    return edh_srv_message_1_handler_failure(
        EDH_SRV_MSG1_HDL_ERR_NULL_CREDENTIALS);
  }
  if (!com_readonly_buffer_is_valid(request.payload)) {
    return edh_srv_message_1_handler_failure(
        EDH_SRV_MSG1_HDL_ERR_INVALID_REQUEST_BUFFER);
  }
  if (!com_writable_buffer_is_writable(response)) {
    return edh_srv_message_1_handler_failure(
        EDH_SRV_MSG1_HDL_ERR_INVALID_RESPONSE_BUFFER);
  }

  struct edhoc_context* edhoc_ctx = calloc(1, sizeof(struct edhoc_context));
  if (!edhoc_ctx) {
    return edh_srv_message_1_handler_failure(
        EDH_SRV_MSG1_HDL_ERR_CALLOC_FAILED);
  }
  int edhoc_api_result = edh_com_setup_context(edhoc_ctx, request.credentials);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    free(edhoc_ctx);
    return edh_srv_message_1_handler_failure(
        EDH_SRV_MSG1_HDL_ERR_EDHOC_CONTEXT_SETUP_FAILED);
  }
  edhoc_api_result = edhoc_message_1_process(edhoc_ctx, request.payload.bytes,
                                             request.payload.length);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    edh_srv_message_1_handler_add_error(
        edhoc_api_result, edhoc_ctx, "Message 1 processing failed", response);
    free(edhoc_ctx);
    return edh_srv_message_1_handler_failure(
        EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED);
  }
  edhoc_api_result = edhoc_message_2_compose(
      edhoc_ctx, response->bytes, response->capacity, &response->length);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    edh_srv_message_1_handler_add_error(edhoc_api_result, edhoc_ctx,
                                        "Message 2 composing failed", response);
    free(edhoc_ctx);
    return edh_srv_message_1_handler_failure(
        EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED);
  }

  return edh_srv_message_1_handler_ok(edhoc_ctx);
}

const char* edh_srv_handle_message_1_status_code_to_string(
    const enum edh_srv_message_1_handler_status status) {
  switch (status) {
  case EDH_SRV_MSG1_HDL_OK:
    return "ok";
  case EDH_SRV_MSG1_HDL_ERR_NULL_CREDENTIALS:
    return "null credentials";
  case EDH_SRV_MSG1_HDL_ERR_INVALID_REQUEST_BUFFER:
    return "invalid request buffer";
  case EDH_SRV_MSG1_HDL_ERR_INVALID_RESPONSE_BUFFER:
    return "invalid response buffer";
  case EDH_SRV_MSG1_HDL_ERR_CALLOC_FAILED:
    return "calloc failed";
  case EDH_SRV_MSG1_HDL_ERR_EDHOC_CONTEXT_SETUP_FAILED:
    return "EDHOC context setup failed";
  case EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED:
    return "EDHOC message 1 process failed";
  case EDH_SRV_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED:
    return "EDHOC message 2 compose failed";
  default:
    return "unknown";
  }
}