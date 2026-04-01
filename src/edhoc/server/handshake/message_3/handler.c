/**
 * @file message_3_handler.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Definition for the Message 3 handler.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_3/handler.h"

#include <edhoc_helpers.h>
#include <edhoc_values.h>
#include <stdbool.h>

#include "edhoc/server/handshake/message_3/errors.h"

static bool edhoc_server_message_3_has_invalid_args(
    const edhoc_server_message_3_request_data_t* request_data,
    const common_response_buffer_t* response_data) {
  if (!request_data ||
      !edhoc_server_common_request_data_is_valid(&request_data->base_data) ||
      !common_response_buffer_is_valid(response_data)) {
    return true;
  }

  const bool message_3_fields_not_extracted =
      !request_data->message_3_extracted_fields;
  const bool edhoc_context_is_null = !request_data->base_data.edhoc_ctx;
  return message_3_fields_not_extracted || edhoc_context_is_null;
}

bool edhoc_server_extract_if_properly_formatted_message_3(
    const uint8_t* request_payload, const size_t request_len,
    const struct edhoc_context* edhoc_ctx,
    struct edhoc_extracted_fields* extracted_fields) {
  if (!request_payload || request_len == 0 || !edhoc_ctx || !extracted_fields) {
    return false;
  }

  *extracted_fields = (struct edhoc_extracted_fields){
      .buffer = request_payload,
      .buffer_size = request_len,
      .edhoc_message_ptr = request_payload,
      .edhoc_message_size = request_len,
  };

  if (edhoc_extract_connection_id(extracted_fields) != EDHOC_SUCCESS) {
    return false;
  }

  return edhoc_connection_id_equal(&extracted_fields->extracted_conn_id,
                                   &edhoc_ctx->private_cid);
}

edhoc_server_message_3_result_t edhoc_server_handle_message_3(
    const edhoc_server_message_3_request_data_t* request_data,
    common_response_buffer_t* response_data) {
  if (edhoc_server_message_3_has_invalid_args(request_data, response_data)) {
    return ESHM3_ERR_INVALID_ARGS;
  }

  struct edhoc_context* edhoc_context = request_data->base_data.edhoc_ctx;
  int edhoc_api_result = EDHOC_SUCCESS;

  edhoc_api_result = edhoc_message_3_process(
      edhoc_context,
      request_data->message_3_extracted_fields->edhoc_message_ptr,
      request_data->message_3_extracted_fields->edhoc_message_size);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    add_message_3_error_to_response(
        edhoc_api_result, "Message 3 processing failed", response_data);
    return ESHM3_ERR_MESSAGE_3_PROCESS_FAILED;
  }

  edhoc_api_result = edhoc_message_4_compose(
      edhoc_context, response_data->payload, response_data->payload_capacity,
      &response_data->payload_length);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    add_message_3_error_to_response(
        edhoc_api_result, "Message 4 composing failed", response_data);
    return ESHM3_ERR_MESSAGE_4_COMPOSE_FAILED;
  }

  return ESHM3_OK;
}
