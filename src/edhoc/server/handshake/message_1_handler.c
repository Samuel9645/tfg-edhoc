/**
 * @file message_1_handler.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since Tue 31 Mar
 * @brief Definition for the Message 1 handler
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1_handler.h"

#include <stdlib.h>

#include "edhoc/common/constants.h"
#include "edhoc/common/setup.h"
#include "edhoc/config.h"
#include "edhoc/server/handle_libedhoc_errors.h"

bool edhoc_server_is_properly_formatted_message_1(const uint8_t* payload,
                                                  const size_t payload_len) {
  return payload != NULL && payload_len > 0 && payload[0] == EDCC_CBOR_TRUE;
}

static bool arguments_are_invalid(const uint8_t** payload,
                                  const size_t* length) {
  return !payload || !*payload || !length || *length == 0;
}

static bool first_byte_is_not_cbor_true(const uint8_t** payload) {
  return (*payload)[0] != EDCC_CBOR_TRUE;
}

edhoc_server_message_1_status_t edhoc_server_remove_cbor_true_prefix(
    const uint8_t** payload, size_t* length) {
  if (arguments_are_invalid(payload, length)) {
    return CSH_ERR_INVALID_ARGS;
  }
  if (first_byte_is_not_cbor_true(payload)) {
    return CSH_ERR_PREFIX_MISSING;
  }

  *payload += 1;
  *length -= 1;
  return CSH_OK;
}

static bool edhoc_server_message_1_has_invalid_args(
    const edhoc_server_common_request_data_t* request_data,
    const common_response_buffer_t* response_data) {
  if (!request_data ||
      !edhoc_server_common_request_data_is_valid(request_data) ||
      !common_response_buffer_is_valid(response_data)) {
    return true;
  }

  const bool session_already_exists = request_data->edhoc_ctx != NULL;
  const bool payload_is_too_short =
      request_data->request_data.payload_length <= 1;
  return session_already_exists || payload_is_too_short;
}

edhoc_server_message_1_result_t edhoc_server_handle_message_1(
    const edhoc_server_message_1_request_data_t* message_1_request_data,
    common_response_buffer_t* response_data) {
  if (!message_1_request_data) {
    return edhoc_server_message_1_failure(CSH_ERR_INVALID_ARGS);
  }
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