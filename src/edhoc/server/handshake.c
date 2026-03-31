#include "edhoc/server/handshake.h"

#include <edhoc_helpers.h>
#include <edhoc_values.h>
#include <stdbool.h>
#include <stdlib.h>

// TODO: remove this include once the refactor is done
#include "coap/server/edhoc_mapper.h"
#include "edhoc/common/constants.h"
#include "edhoc/common/setup.h"
#include "edhoc/edhoc_config.h"
#include "edhoc/server/handle_libedhoc_errors.h"
#include "edhoc/server/handshake_result.h"

static inline bool params_are_invalid(
    const edhoc_server_common_request_data_t* base_data,
    const common_response_buffer_t* response_data) {
  return !edhoc_server_common_request_data_is_valid(base_data) ||
         !common_response_buffer_is_valid(response_data);
}

static inline bool edhoc_server_message_3_has_invalid_args(
    const edhoc_server_message_3_request_data_t* request_data,
    const common_response_buffer_t* response_data) {
  if (!request_data ||
      params_are_invalid(&request_data->base_data, response_data)) {
    return true;
  }

  const bool message_3_fields_not_extracted =
      !request_data->message_3_extracted_fields;
  const bool edhoc_context_is_null = !request_data->base_data.edhoc_ctx;
  return message_3_fields_not_extracted || edhoc_context_is_null;
}

bool edhoc_server_is_properly_formatted_message_1(const uint8_t* payload,
                                                  const size_t payload_len) {
  return payload != NULL && payload_len > 0 && payload[0] == EDCC_CBOR_TRUE;
}

edhoc_server_handshake_status_t edhoc_server_remove_cbor_true_prefix(
    const uint8_t** payload, size_t* length) {
  const bool payload_is_invalid = (!*payload || !length || *length == 0);
  if (payload_is_invalid) {
    return CSH_ERR_INVALID_ARGS;
  }
  const bool first_byte_is_not_cbor_true = (*payload)[0] != EDCC_CBOR_TRUE;
  if (first_byte_is_not_cbor_true) {
    return CSH_ERR_PREFIX_MISSING;
  }

  *payload += 1;
  *length -= 1;
  return CSH_OK;
}

static inline bool edhoc_server_message_1_has_invalid_args(
    const edhoc_server_common_request_data_t* request_data,
    const common_response_buffer_t* response_data) {
  if (params_are_invalid(request_data, response_data)) {
    return true;
  }

  const bool session_already_exists = (request_data->edhoc_ctx != NULL);
  const bool payload_is_too_short =
      (request_data->request_data.payload_length <= 1);
  return session_already_exists || payload_is_too_short;
}

edhoc_server_message_1_result_t edhoc_server_handle_message_1(
    const edhoc_server_message_1_request_data_t* message_1_request_data,
    common_response_buffer_t* response_data) {
  const edhoc_server_common_request_data_t* base_data =
      &message_1_request_data->base_data;
  if (edhoc_server_message_1_has_invalid_args(base_data, response_data)) {
    return edhoc_server_message_1_failure(CSH_ERR_INVALID_ARGS);
  }

  if (base_data->request_data.payload_length > EDC_MESSAGE_BUFFER_LENGTH) {
    return edhoc_server_message_1_failure(CSH_ERR_PAYLOAD_TOO_LARGE);
  }
  const uint8_t* no_prefix_payload = base_data->request_data.payload;
  size_t no_prefix_payload_len = base_data->request_data.payload_length;
  if (edhoc_server_remove_cbor_true_prefix(&no_prefix_payload,
                                           &no_prefix_payload_len) != CSH_OK) {
    return edhoc_server_message_1_failure(CSH_ERR_PREFIX_MISSING);
  }

  struct edhoc_context* edhoc_ctx = calloc(1, sizeof(struct edhoc_context));
  if (!edhoc_ctx) {
    return edhoc_server_message_1_failure(CSH_ERR_CALLOC_FAILED);
  }

  int edhoc_api_result = edhoc_common_setup_context(
      edhoc_ctx, message_1_request_data->credentials);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    free(edhoc_ctx);
    return edhoc_server_message_1_failure(CSH_ERR_EDHOC_CONTEXT_SETUP_FAILED);
  }

  edhoc_api_result = edhoc_message_1_process(edhoc_ctx, no_prefix_payload,
                                             no_prefix_payload_len);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    server_edhoc_add_edhoc_error_to_response(edhoc_api_result, edhoc_ctx,
                                             response_data);
    free(edhoc_ctx);
    return edhoc_server_message_1_failure(
        CSH_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED);
  }

  edhoc_api_result = edhoc_message_2_compose(edhoc_ctx, response_data->payload,
                                             response_data->payload_capacity,
                                             &response_data->payload_length);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    server_edhoc_add_edhoc_error_to_response(edhoc_api_result, edhoc_ctx,
                                             response_data);
    free(edhoc_ctx);
    return edhoc_server_message_1_failure(
        CSH_ERR_EDHOC_MESSAGE_2_COMPOSE_FAILED);
  }

  return edhoc_server_message_1_ok(edhoc_ctx);
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
