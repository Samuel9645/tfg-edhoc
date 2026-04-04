#include "core/client.h"

#include <coap3/coap.h>

#include "coap/client/cleanup.h"
#include "coap/client/exchange.h"
#include "coap/client/utils.h"
#include "coap/config.h"
#include "edhoc/client/handshake.h"
#include "edhoc/config.h"
#include "edhoc/credentials/authentication.h"
#include "edhoc/credentials/client_private_key.h"
#include "edhoc/credentials/public_data.h"

static int client_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  return edh_cred_auth_fetch(
      user_context, credentials, EDH_CRED_PUB_CLI_PK,
      ARRAY_SIZE(EDH_CRED_PUB_CLI_PK), EDH_CRED_CLI_PRIVATE_KEY,
      ARRAY_SIZE(EDH_CRED_CLI_PRIVATE_KEY), EDH_CRED_PUB_CLI_KID);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef (libedhoc signature
// forces it)
static int client_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  return edh_cred_auth_verify(
      user_context, credentials, EDH_CRED_PUB_SRV_KID, EDH_CRED_PUB_SRV_PK,
      ARRAY_SIZE(EDH_CRED_PUB_SRV_PK), public_key_reference, public_key_length);
}

static const struct edhoc_credentials credentials = {
    .fetch = client_credential_fetch,
    .verify = client_credential_verify,
};

static void cp_cli_try_send_edhoc_error_payload(
    cp_cli_session_resources_t* client_resources,
    cp_cli_exchange_request_data_t* request_data,
    const com_writable_buffer_t* error_payload_data,
    com_writable_buffer_t* receive_buffer) {
  if (client_resources == NULL || request_data == NULL ||
      error_payload_data == NULL || receive_buffer == NULL ||
      !com_writable_buffer_has_content(error_payload_data)) {
    return;
  }

  request_data->request_data.length = error_payload_data->length;
  request_data->content_format = CP_CFG_CONTENT_CID_EDHOC;

  cp_cli_exchange_reset(&client_resources->exchange);
  (void)cp_cli_exchange_send(&client_resources->exchange, request_data);
  (void)cp_cli_exchange_wait_and_get(&client_resources->exchange,
                                     receive_buffer);
}

com_emulation_status_t core_run_client(void) {
  coap_startup();
  coap_set_log_level(COAP_LOG_DEBUG);

  cp_cli_session_resources_t client_resources = {0};

  static const char CLIENT_COAP_URI[] =
      "coap://localhost:5683/.well-known/edhoc";
  coap_uri_t client_uri = {0};
  coap_address_t destination_address = {0};
  if (cp_cli_parse_and_resolve_coap_uri(CLIENT_COAP_URI, &client_uri,
                                        &destination_address) !=
      CP_STATUS_SUCCESS) {
    coap_log_err("Failed to parse or resolve CoAP URI\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  if (cp_cli_create_coap_session(
          &client_uri, &destination_address, NULL,
          &client_resources.session_resources.coap_context,
          &client_resources.session_resources.coap_session) !=
      CP_STATUS_SUCCESS) {
    coap_log_err("Failed to create CoAP session\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  cp_cli_exchange_session_data_t exchange_session_data = {
      .context = client_resources.session_resources.coap_context,
      .session = client_resources.session_resources.coap_session,
      .uri = client_uri,
      .destination = destination_address,
  };

  if (cp_cli_init_exchange(&exchange_session_data,
                           &client_resources.exchange) != CP_STATUS_SUCCESS) {
    coap_log_err("Failed to initialize CoAP exchange\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  if (edh_cli_init_handshake(&client_resources.handshake, &credentials) !=
      EDH_CLI_INIT_OK) {
    coap_log_err("Failed to initialize EDHOC handshake\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  uint8_t request_payload[EDH_CFG_MESSAGE_BUFFER_LENGTH] = {0};
  uint8_t response_payload[CP_CFG_MAX_PDU_SIZE] = {0};

  cp_cli_exchange_request_data_t request_data = {
      .request_data =
          {
              .bytes = request_payload,
              .length = 0,
          },
      .content_format = CP_CFG_CONTENT_CID_EDHOC,
  };

  com_writable_buffer_t response_data = {
      .bytes = response_payload,
      .capacity = CP_CFG_MAX_PDU_SIZE,
      .length = 0,
  };

  com_writable_buffer_t request_output = {
      .bytes = request_payload,
      .capacity = EDH_CFG_MESSAGE_BUFFER_LENGTH,
      .length = 0,
  };

  const edh_cli_message_1_result_t message_1_result =
      edh_cli_handshake_compose_message_1(&client_resources.handshake,
                                          &request_output);
  if (message_1_result.status != EDH_CLI_MSG1_COMPOSE_OK) {
    coap_log_err("Failed to compose EDHOC message 1\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  request_data.request_data.length = message_1_result.output.length;

  if (cp_cli_exchange_send(&client_resources.exchange, &request_data) !=
      CP_STATUS_SUCCESS) {
    coap_log_err("Failed to send CoAP request for message 1\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  if (cp_cli_exchange_wait_and_get(&client_resources.exchange,
                                   &response_data) != CP_STATUS_SUCCESS) {
    coap_log_err("Failed to receive EDHOC message 2\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  const com_readonly_buffer_t message_2_input = {
      .bytes = response_payload,
      .length = response_data.length,
  };
  request_output.length = 0;
  const edh_cli_message_2_result_t message_2_result =
      edh_cli_handshake_process_message_2(&client_resources.handshake,
                                          &message_2_input, &request_output);
  if (message_2_result.status != EDH_CLI_MSG2_PROCESS_OK) {
    coap_log_err("Failed to receive or process EDHOC message 2\n");
    cp_cli_try_send_edhoc_error_payload(&client_resources, &request_data,
                                        &message_2_result.output,
                                        &response_data);
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  cp_cli_exchange_reset(&client_resources.exchange);

  request_output.length = 0;
  const edh_cli_message_3_result_t message_3_result =
      edh_cli_handshake_compose_message_3(&client_resources.handshake,
                                          &request_output);
  if (message_3_result.status != EDH_CLI_MSG3_COMPOSE_OK) {
    coap_log_err("Failed to compose EDHOC message 3\n");
    cp_cli_try_send_edhoc_error_payload(&client_resources, &request_data,
                                        &message_3_result.output,
                                        &response_data);
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  request_data.request_data.length = message_3_result.output.length;

  if (cp_cli_exchange_send(&client_resources.exchange, &request_data) !=
      CP_STATUS_SUCCESS) {
    coap_log_err("Failed to send EDHOC message 3\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  if (cp_cli_exchange_wait_and_get(&client_resources.exchange,
                                   &response_data) != CP_STATUS_SUCCESS) {
    coap_log_err("Failed to receive EDHOC message 4\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  const com_readonly_buffer_t message_4_input = {
      .bytes = response_payload,
      .length = response_data.length,
  };
  request_output.length = 0;
  const edh_cli_message_4_result_t message_4_result =
      edh_cli_handshake_process_message_4(&client_resources.handshake,
                                          &message_4_input, &request_output);
  if (message_4_result.status != EDH_CLI_MSG4_PROCESS_OK) {
    coap_log_err("Failed to process EDHOC message 4\n");
    cp_cli_try_send_edhoc_error_payload(&client_resources, &request_data,
                                        &message_4_result.output,
                                        &response_data);
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  cp_cli_cleanup_resources(&client_resources);
  return COM_EMULATION_SUCCESS;
}