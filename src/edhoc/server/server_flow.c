#include "edhoc/server/server_flow.h"

#include <edhoc_helpers.h>
#include <edhoc_values.h>
#include <stdbool.h>
#include <stdlib.h>

#include "coap/shared/edhoc_error_map.h"
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

coap_pdu_code_t server_edhoc_handle_message_1(
    const server_edhoc_message_1_request_data_t* request_data,
    const coap_response_data_t* response_data) {
  if (!request_data || !response_data || !request_data->session ||
      !request_data->response || !request_data->request_data.payload ||
      request_data->request_data.payload_len <= 1 || !response_data->payload ||
      !response_data->payload_len) {
    return COAP_RESPONSE_CODE_BAD_REQUEST;
  }

  const uint8_t* request_payload = request_data->request_data.payload;
  size_t request_len = request_data->request_data.payload_len;

  struct edhoc_context* edhoc_ctx =
      (struct edhoc_context*)coap_session_get_app_data(request_data->session);
  if (edhoc_ctx != NULL) {
    coap_log_err("EDHOC context already exists for this session\n");
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }

  const uint8_t* edhoc_msg1_bytes = request_payload + 1;
  size_t edhoc_msg1_len = request_len - 1;

  edhoc_ctx = calloc(1, sizeof(struct edhoc_context));
  if (!edhoc_ctx) {
    coap_log_err("cannot allocate memory for EDHOC context\n");
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }

  if (edhoc_setup_context(edhoc_ctx, &SERVER_CREDENTIALS) != EDHOC_SUCCESS) {
    coap_log_err("cannot setup EDHOC context\n");
    free(edhoc_ctx);
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }

  if (coap_session_set_app_data2(request_data->session, edhoc_ctx, free) !=
      NULL) {
    coap_log_err("app data for session already set\n");
    free(edhoc_ctx);
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }

  if (edhoc_message_1_process(edhoc_ctx, edhoc_msg1_bytes, edhoc_msg1_len) !=
      EDHOC_SUCCESS) {
    coap_shared_map_edhoc_failure_to_response(edhoc_ctx, "process Message 1",
                                              true, request_data->response);
    return coap_pdu_get_code(request_data->response);
  }

  if (edhoc_message_2_compose(edhoc_ctx, response_data->payload,
                              response_data->payload_capacity,
                              response_data->payload_len) != EDHOC_SUCCESS) {
    coap_shared_map_edhoc_failure_to_response(edhoc_ctx, "compose Message 2",
                                              false, request_data->response);
    return coap_pdu_get_code(request_data->response);
  }

  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t server_edhoc_handle_message_3(
    const server_edhoc_message_3_request_data_t* request_data,
    const coap_response_data_t* response_data) {
  if (!request_data || !response_data || !request_data->session ||
      !request_data->response || !request_data->request_data.payload ||
      request_data->request_data.payload_len == 0 || !response_data->payload ||
      !response_data->payload_len || !request_data->extracted_fields) {
    return COAP_RESPONSE_CODE_BAD_REQUEST;
  }

  struct edhoc_context* edhoc_ctx =
      (struct edhoc_context*)coap_session_get_app_data(request_data->session);
  if (!edhoc_ctx) {
    coap_log_err("no EDHOC context for this session\n");
    return COAP_RESPONSE_CODE_BAD_REQUEST;
  }

  if (edhoc_message_3_process(
          edhoc_ctx, request_data->extracted_fields->edhoc_message_ptr,
          request_data->extracted_fields->edhoc_message_size) !=
      EDHOC_SUCCESS) {
    coap_shared_map_edhoc_failure_to_response(edhoc_ctx, "process Message 3",
                                              true, request_data->response);
    return coap_pdu_get_code(request_data->response);
  }

  if (edhoc_message_4_compose(edhoc_ctx, response_data->payload,
                              response_data->payload_capacity,
                              response_data->payload_len) != EDHOC_SUCCESS) {
    coap_shared_map_edhoc_failure_to_response(edhoc_ctx, "compose Message 4",
                                              false, request_data->response);
    return coap_pdu_get_code(request_data->response);
  }

  return COAP_RESPONSE_CODE_CHANGED;
}
