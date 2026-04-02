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

static bool message_1_has_invalid_args(
    const edh_srv_request_t* request_data,
    const com_response_buffer_t* response_data) {
  if (!request_data || !edh_srv_request_data_is_valid(request_data) ||
      !com_response_buffer_is_valid(response_data)) {
    return true;
  }

  const bool session_already_exists = request_data->edhoc_ctx != NULL;
  return session_already_exists;
}

ehd_message_1_handler_result_t edh_srv_handle_message_1(
    const edh_srv_message_1_request_t* message_1_request_data,
    com_response_buffer_t* response_data) {
  if (!message_1_request_data) {
    return edh_message_1_handler_failure(EDH_MSG1_HDL_ERR_INVALID_ARGS);
  }
  const edh_srv_request_t* base_data = &message_1_request_data->base_data;
  if (message_1_has_invalid_args(base_data, response_data)) {
    return edh_message_1_handler_failure(EDH_MSG1_HDL_ERR_INVALID_ARGS);
  }

  if (base_data->request_data.payload_length > EDH_CFG_MESSAGE_BUFFER_LENGTH) {
    return edh_message_1_handler_failure(EDH_MSG1_HDL_ERR_PAYLOAD_TOO_LARGE);
  }

  struct edhoc_context* edhoc_ctx = calloc(1, sizeof(struct edhoc_context));
  if (!edhoc_ctx) {
    return edh_message_1_handler_failure(EDH_MSG1_HDL_ERR_CALLOC_FAILED);
  }

  int edhoc_api_result =
      edh_com_setup_context(edhoc_ctx, message_1_request_data->credentials);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    free(edhoc_ctx);
    return edh_message_1_handler_failure(
        EDH_MSG1_HDL_ERR_EDHOC_CONTEXT_SETUP_FAILED);
  }

  edhoc_api_result =
      edhoc_message_1_process(edhoc_ctx, base_data->request_data.payload,
                              base_data->request_data.payload_length);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    tst_edh_message_1_handler_add_error(edhoc_api_result, edhoc_ctx,
                                        "Message 1 processing failed",
                                    response_data);
    free(edhoc_ctx);
    return edh_message_1_handler_failure(
        EDH_MSG1_HDL_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED);
  }

  edhoc_api_result = edhoc_message_2_compose(edhoc_ctx, response_data->payload,
                                             response_data->payload_capacity,
                                             &response_data->payload_length);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    tst_edh_message_1_handler_add_error(edhoc_api_result, edhoc_ctx,
                                        "Message 2 composing failed",
                                    response_data);
    free(edhoc_ctx);
    return edh_message_1_handler_failure(
        EDH_MSG1_HDL_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED);
  }

  return edh_message_1_handler_ok(edhoc_ctx);
}