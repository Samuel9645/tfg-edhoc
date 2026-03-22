#include "edhoc/server/server_flow.h"

#include <edhoc_helpers.h>
#include <edhoc_values.h>
#include <stdbool.h>
#include <stdlib.h>

#include "coap/server/map_edhoc_error_to_response.h"
#include "coap/shared/edhoc_request.h"
#include "edhoc/common/setup.h"
#include "edhoc/credentials/authentication.h"
#include "edhoc/credentials/public_data.h"
#include "edhoc/credentials/server_private_key.h"

static int server_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  return credential_fetch(user_context, credentials, SERVER_PUBLIC_KEY,
                          ARRAY_SIZE(SERVER_PUBLIC_KEY), SERVER_PRIVATE_KEY,
                          ARRAY_SIZE(SERVER_PRIVATE_KEY), SERVER_KID);
}

static int server_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  return credential_verify(user_context, credentials, CLIENT_KID,
                           CLIENT_PUBLIC_KEY, ARRAY_SIZE(CLIENT_PUBLIC_KEY),
                           public_key_reference, public_key_length);
}

static const struct edhoc_credentials SERVER_CREDENTIALS = {
    .fetch = server_credential_fetch,
    .verify = server_credential_verify,
};

static inline bool server_edhoc_has_invalid_common_request_data(
    const server_edhoc_common_request_data_t* base_data) {
  return !base_data || !base_data->session || !base_data->response ||
         !base_data->request_data.payload ||
         base_data->request_data.payload_len == 0;
}

static inline bool server_edhoc_has_invalid_response_data(
    const coap_response_data_t* response_data) {
  return !response_data || !response_data->payload ||
         !response_data->payload_len;
}

static inline bool server_edhoc_message_1_has_invalid_args(
    const server_edhoc_message_1_request_data_t* request_data,
    const coap_response_data_t* response_data) {
  return !request_data ||
         server_edhoc_has_invalid_common_request_data(
             &request_data->base_data) ||
         server_edhoc_has_invalid_response_data(response_data) ||
         request_data->base_data.edhoc_ctx != NULL ||
         request_data->base_data.request_data.payload_len <= 1;
}

static inline bool server_edhoc_message_3_has_invalid_args(
    const server_edhoc_message_3_request_data_t* request_data,
    const coap_response_data_t* response_data) {
  return !request_data ||
         server_edhoc_has_invalid_common_request_data(
             &request_data->base_data) ||
         server_edhoc_has_invalid_response_data(response_data) ||
         !request_data->base_data.edhoc_ctx ||
         !request_data->message_3_extracted_fields;
}

coap_pdu_code_t server_edhoc_handle_message_1(
    const server_edhoc_message_1_request_data_t* request_data,
    coap_response_data_t* response_data) {
  if (server_edhoc_message_1_has_invalid_args(request_data, response_data)) {
    return COAP_RESPONSE_CODE_BAD_REQUEST;
  }

  const uint8_t* request_payload = request_data->base_data.request_data.payload;
  size_t request_len = request_data->base_data.request_data.payload_len;

  struct edhoc_context* edhoc_context = NULL;

  const uint8_t* edhoc_msg1_bytes = request_payload + 1;
  size_t edhoc_msg1_len = request_len - 1;

  edhoc_context = calloc(1, sizeof(struct edhoc_context));
  if (!edhoc_context) {
    coap_log_err("cannot allocate memory for EDHOC context\n");
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }

  int edhoc_api_result =
      edhoc_setup_context(edhoc_context, &SERVER_CREDENTIALS);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    coap_pdu_code_t response_code = coap_server_map_edhoc_failure_to_response(
        edhoc_context, "setup EDHOC context", COAP_SERVER_EDHOC_INTERNAL_ERROR,
        edhoc_api_result, request_data->base_data.response, response_data);
    free(edhoc_context);
    return response_code;
  }

  if (coap_session_set_app_data2(request_data->base_data.session, edhoc_context,
                                 free) != NULL) {
    coap_log_err("app data for session already set\n");
    free(edhoc_context);
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }

  edhoc_api_result =
      edhoc_message_1_process(edhoc_context, edhoc_msg1_bytes, edhoc_msg1_len);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    return coap_server_map_edhoc_failure_to_response(
        edhoc_context, "process Message 1", COAP_SERVER_EDHOC_PROTOCOL_ERROR,
        edhoc_api_result, request_data->base_data.response, response_data);
  }

  edhoc_api_result = edhoc_message_2_compose(
      edhoc_context, response_data->payload, response_data->payload_capacity,
      response_data->payload_len);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    return coap_server_map_edhoc_failure_to_response(
        edhoc_context, "compose Message 2", COAP_SERVER_EDHOC_INTERNAL_ERROR,
        edhoc_api_result, request_data->base_data.response, response_data);
  }

  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t server_edhoc_handle_message_3(
    const server_edhoc_message_3_request_data_t* request_data,
    coap_response_data_t* response_data) {
  if (server_edhoc_message_3_has_invalid_args(request_data, response_data)) {
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
      response_data->payload_len);
  if (edhoc_api_result != EDHOC_SUCCESS) {
    return coap_server_map_edhoc_failure_to_response(
        edhoc_context, "compose Message 4", COAP_SERVER_EDHOC_INTERNAL_ERROR,
        edhoc_api_result, request_data->base_data.response, response_data);
  }

  return COAP_RESPONSE_CODE_CHANGED;
}
