#include "edhoc/server/handshake.h"

#include <edhoc_helpers.h>
#include <edhoc_values.h>
#include <stdbool.h>
#include <stdlib.h>

// TODO: remove this include once the refactor is done
#include "coap/server/edhoc_mapper.h"
#include "edhoc/common/setup.h"
#include "edhoc/server/handle_libedhoc_errors.h"

static inline bool edhoc_server_message_3_has_invalid_args(
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

coap_pdu_code_t edhoc_server_handle_message_3(
    const edhoc_server_message_3_request_data_t* request_data,
    common_response_buffer_t* response_data) {
  if (edhoc_server_message_3_has_invalid_args(request_data, response_data)) {
    return COAP_RESPONSE_CODE_BAD_REQUEST;
  }

  struct edhoc_context* edhoc_context = request_data->base_data.edhoc_ctx;
  int edhoc_api_result = EDHOC_SUCCESS;

  edhoc_api_result = edhoc_message_3_process(
      edhoc_context,
      request_data->message_3_extracted_fields->edhoc_message_ptr,
      request_data->message_3_extracted_fields->edhoc_message_size);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    return coap_server_map_edhoc_failure_to_response(
        edhoc_context, "process Message 3", COAP_SERVER_EDHOC_PROTOCOL_ERROR,
        edhoc_api_result, request_data->base_data.response, response_data);
  }

  edhoc_api_result = edhoc_message_4_compose(
      edhoc_context, response_data->payload, response_data->payload_capacity,
      &response_data->payload_length);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    return coap_server_map_edhoc_failure_to_response(
        edhoc_context, "compose Message 4", COAP_SERVER_EDHOC_INTERNAL_ERROR,
        edhoc_api_result, request_data->base_data.response, response_data);
  }

  return COAP_RESPONSE_CODE_CHANGED;
}
