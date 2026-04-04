/**
 * @file handler.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Definition for the Message 1 handler
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/handler.h"

#include <edhoc.h>
#include <stdlib.h>

#include "edhoc/common/setup.h"
#include "edhoc/config.h"
#include "edhoc/server/handshake/message_1/errors.h"

static bool request_is_valid(const edh_srv_message_1_request_t* request) {
  return request && request->credentials &&
         com_readonly_buffer_is_valid(&request->payload);
}

static bool message_1_handler_has_valid_args(
    const edh_srv_message_1_request_t* request,
    const com_writable_buffer_t* response) {
  return request_is_valid(request) && com_writable_buffer_is_writable(response);
}

ehd_srv_message_1_handler_result_t edh_srv_handle_message_1(
    const edh_srv_message_1_request_t* request,
    com_writable_buffer_t* response) {
  if (!message_1_handler_has_valid_args(request, response)) {
    return edh_srv_message_1_handler_failure(EDH_SRV_MSG1_HDL_ERR_INVALID_ARGS);
  }

  if (request->payload.length > EDH_CFG_MESSAGE_BUFFER_LENGTH) {
    return edh_srv_message_1_handler_failure(EDH_SRV_MSG1_HDL_ERR_PAYLOAD_TOO_LARGE);
  }

  struct edhoc_context* edhoc_ctx = calloc(1, sizeof(struct edhoc_context));
  if (!edhoc_ctx) {
    return edh_srv_message_1_handler_failure(EDH_SRV_MSG1_HDL_ERR_CALLOC_FAILED);
  }

  int edhoc_api_result = edh_com_setup_context(edhoc_ctx, request->credentials);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    free(edhoc_ctx);
    return edh_srv_message_1_handler_failure(
        EDH_SRV_MSG1_HDL_ERR_EDHOC_CONTEXT_SETUP_FAILED);
  }

  edhoc_api_result = edhoc_message_1_process(edhoc_ctx, request->payload.bytes,
                                             request->payload.length);
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