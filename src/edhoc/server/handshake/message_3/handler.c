/**
 * @file handler.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Definition for the Message 3 handler.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_3/handler.h"

#include <edhoc.h>
#include <stdbool.h>

#include "edhoc/server/handshake/message_3/errors.h"

static bool message_3_has_invalid_args(
    const edh_srv_message_3_request_t* request_data,
    const com_response_buffer_t* response_data) {
  if (!request_data ||
      !edh_srv_request_data_is_valid(&request_data->base_data) ||
      !com_response_buffer_is_valid(response_data)) {
    return true;
  }

  const bool message_3_fields_not_extracted =
      !request_data->message_3_extracted_fields;
  const bool edhoc_context_is_null = !request_data->base_data.edhoc_ctx;
  return message_3_fields_not_extracted || edhoc_context_is_null;
}

edh_message_3_handler_status_t edh_srv_handle_message_3(
    const edh_srv_message_3_request_t* request_data,
    com_response_buffer_t* response_data) {
  if (message_3_has_invalid_args(request_data, response_data)) {
    return EDH_MSG3_HDL_ERR_INVALID_ARGS;
  }

  struct edhoc_context* edhoc_context = request_data->base_data.edhoc_ctx;
  int edhoc_api_result = EDHOC_SUCCESS;

  edhoc_api_result = edhoc_message_3_process(
      edhoc_context,
      request_data->message_3_extracted_fields->edhoc_message_ptr,
      request_data->message_3_extracted_fields->edhoc_message_size);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    edh_message_3_handler_add_error(
        edhoc_api_result, "Message 3 processing failed", response_data);
    return EDH_MSG3_HDL_ERR_MESSAGE_3_PROCESS_FAILED;
  }

  edhoc_api_result = edhoc_message_4_compose(
      edhoc_context, response_data->payload, response_data->payload_capacity,
      &response_data->payload_length);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    edh_message_3_handler_add_error(
        edhoc_api_result, "Message 4 composing failed", response_data);
    return EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_FAILED;
  }

  return EDH_MSG3_HDL_OK;
}
