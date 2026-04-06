#include "core/client.h"

#include <coap3/coap.h>

#include "coap/client/cleanup.h"
#include "coap/client/cli_utils.h"
#include "coap/client/exchange.h"
#include "coap/common/cp_create_context.h"
#include "coap/config.h"
#include "edhoc/client/cli_handshake.h"
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
    struct cp_cli_session_resources* client_resources,
    struct cp_cli_exchange_request request_data,
    const struct com_writable_buffer* error_payload_data,
    struct com_writable_buffer* receive_buffer) {
  if (client_resources == NULL || error_payload_data == NULL ||
      receive_buffer == NULL ||
      !com_writable_buffer_has_content(error_payload_data)) {
    return;
  }

  request_data.buffer.length = error_payload_data->length;
  request_data.content_format = CP_CFG_CONTENT_CID_EDHOC;

  cp_cli_exchange_reset(&client_resources->exchange);
  (void)cp_cli_exchange_send(&client_resources->exchange, request_data);
  (void)cp_cli_exchange_wait_and_get(&client_resources->exchange,
                                     receive_buffer);
}

enum com_emulation_status core_run_client(void) {
  coap_startup();
  coap_set_log_level(COAP_LOG_DEBUG);

  struct cp_cli_session_resources client_resources = {0};

  static const char CLIENT_COAP_URI[] =
      "coap://localhost:5683/.well-known/edhoc";

  struct cp_cli_parse_and_resolve_result parse_and_resolve_uri_result =
      cp_cli_parse_and_resolve_coap_uri(CLIENT_COAP_URI);
  if (parse_and_resolve_uri_result.status != CP_PARSE_AND_RESOLVE_OK) {
    return COM_EMULATION_FAILURE;
  }
  const

      struct cp_com_create_context_result create_context_result =
          cp_com_create_context();
  if (create_context_result.status != CP_COM_INIT_OK) {
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  client_resources.session_resources.coap_context =
      create_context_result.context;
  struct cp_cli_session_config config = {
      .address = &parse_and_resolve_uri_result.address,
      .uri = &parse_and_resolve_uri_result.uri,
  };
  struct cp_cli_create_session_result create_session_result =
      cp_cli_create_session(create_context_result.context, config);
  if (create_session_result.status != CP_CLI_CREATE_SESSION_OK) {
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  client_resources.session_resources.coap_session =
      create_session_result.session;

  struct cp_cli_exchange_session_data exchange_session_data = {
      .context = client_resources.session_resources.coap_context,
      .session = client_resources.session_resources.coap_session,
      .uri = parse_and_resolve_uri_result.uri,
      .destination = parse_and_resolve_uri_result.address,
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

  uint8_t request_payload[CP_CFG_MAX_PDU_SIZE] = {0};
  uint8_t response_payload[CP_CFG_MAX_PDU_SIZE] = {0};

  struct cp_cli_exchange_request request_data = {
      .buffer =
          {
              .bytes = request_payload,
              .length = 0,
          },
      .content_format = CP_CFG_CONTENT_CID_EDHOC,
  };

  struct com_writable_buffer response_data = {
      .bytes = response_payload,
      .capacity = CP_CFG_MAX_PDU_SIZE,
      .length = 0,
  };

  struct com_writable_buffer request_output = {
      .bytes = request_payload,
      .capacity = CP_CFG_MAX_PDU_SIZE,
      .length = 0,
  };

  const struct edh_cli_message_1_result message_1_result =
      edh_cli_handshake_compose_message_1(&client_resources.handshake,
                                          &request_output);
  if (message_1_result.status != EDH_CLI_MSG1_COMPOSE_OK) {
    coap_log_err("Failed to compose EDHOC message 1\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  request_data.buffer.length = message_1_result.output.length;

  if (cp_cli_exchange_send(&client_resources.exchange, request_data) !=
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

  const struct com_readonly_buffer message_2_input = {
      .bytes = response_payload,
      .length = response_data.length,
  };
  request_output.length = 0;
  const struct edh_cli_message_2_result message_2_result =
      edh_cli_handshake_process_message_2(&client_resources.handshake,
                                          message_2_input, &request_output);
  if (message_2_result.status != EDH_CLI_MSG2_PROCESS_OK) {
    coap_log_err("Failed to receive or process EDHOC message 2\n");
    cp_cli_try_send_edhoc_error_payload(&client_resources, request_data,
                                        &message_2_result.output,
                                        &response_data);
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  cp_cli_exchange_reset(&client_resources.exchange);

  request_output.length = 0;
  const struct edh_cli_message_3_result message_3_result =
      edh_cli_handshake_compose_message_3(&client_resources.handshake,
                                          &request_output);
  if (message_3_result.status != EDH_CLI_MSG3_COMPOSE_OK) {
    coap_log_err("Failed to compose EDHOC message 3\n");
    cp_cli_try_send_edhoc_error_payload(&client_resources, request_data,
                                        &message_3_result.output,
                                        &response_data);
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  request_data.buffer.length = message_3_result.output.length;

  if (cp_cli_exchange_send(&client_resources.exchange, request_data) !=
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

  const struct com_readonly_buffer message_4_input = {
      .bytes = response_payload,
      .length = response_data.length,
  };
  request_output.length = 0;
  const struct edh_cli_message_4_result message_4_result =
      edh_cli_handshake_process_message_4(&client_resources.handshake,
                                          message_4_input, &request_output);
  if (message_4_result.status != EDH_CLI_MSG4_PROCESS_OK) {
    coap_log_err("Failed to process EDHOC message 4\n");
    cp_cli_try_send_edhoc_error_payload(&client_resources, request_data,
                                        &message_4_result.output,
                                        &response_data);
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  cp_cli_cleanup_resources(&client_resources);
  return COM_EMULATION_SUCCESS;
}