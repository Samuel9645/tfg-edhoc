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
    coap_session_t* session, const uint8_t* request_payload, size_t request_len,
    size_t response_capacity, coap_pdu_t* response, uint8_t* response_payload,
    size_t* response_len) {
  if (!session || !request_payload || request_len <= 1 || !response_payload ||
      !response_len || !response) {
    return COAP_RESPONSE_CODE_BAD_REQUEST;
  }

  struct edhoc_context* edhoc_ctx =
      (struct edhoc_context*)coap_session_get_app_data(session);
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

  if (coap_session_set_app_data2(session, edhoc_ctx, free) != NULL) {
    coap_log_err("app data for session already set\n");
    free(edhoc_ctx);
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }

  if (edhoc_message_1_process(edhoc_ctx, edhoc_msg1_bytes, edhoc_msg1_len) !=
      EDHOC_SUCCESS) {
    coap_shared_map_edhoc_failure_to_response(edhoc_ctx, "process Message 1",
                                              true, response);
    return coap_pdu_get_code(response);
  }

  if (edhoc_message_2_compose(edhoc_ctx, response_payload, response_capacity,
                              response_len) != EDHOC_SUCCESS) {
    coap_shared_map_edhoc_failure_to_response(edhoc_ctx, "compose Message 2",
                                              false, response);
    return coap_pdu_get_code(response);
  }

  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t server_edhoc_handle_message_3(
    coap_session_t* session, const uint8_t* request_payload, size_t request_len,
    size_t response_capacity, struct edhoc_extracted_fields* extracted_fields,
    coap_pdu_t* response, uint8_t* response_payload, size_t* response_len) {
  if (!session || !request_payload || request_len == 0 || !response_payload ||
      !response_len || !response || !extracted_fields) {
    return COAP_RESPONSE_CODE_BAD_REQUEST;
  }

  struct edhoc_context* edhoc_ctx =
      (struct edhoc_context*)coap_session_get_app_data(session);
  if (!edhoc_ctx) {
    coap_log_err("no EDHOC context for this session\n");
    return COAP_RESPONSE_CODE_BAD_REQUEST;
  }

  if (edhoc_message_3_process(edhoc_ctx, extracted_fields->edhoc_message_ptr,
                              extracted_fields->edhoc_message_size) !=
      EDHOC_SUCCESS) {
    coap_shared_map_edhoc_failure_to_response(edhoc_ctx, "process Message 3",
                                              true, response);
    return coap_pdu_get_code(response);
  }

  if (edhoc_message_4_compose(edhoc_ctx, response_payload, response_capacity,
                              response_len) != EDHOC_SUCCESS) {
    coap_shared_map_edhoc_failure_to_response(edhoc_ctx, "compose Message 4",
                                              false, response);
    return coap_pdu_get_code(response);
  }

  return COAP_RESPONSE_CODE_CHANGED;
}
