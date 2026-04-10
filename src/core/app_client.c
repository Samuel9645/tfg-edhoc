#include "app/app_client.h"

#include <coap3/coap.h>
#include <string.h>

#include "coap/client/cli_exchange.h"
#include "coap/client/cli_resources.h"
#include "coap/client/cli_utils.h"
#include "coap/coap_config.h"
#include "coap/common/coap_context.h"
#include "common/com_emulation.h"
#include "common/com_session_resources.h"
#include "edhoc/client/cli_handshake.h"
#include "edhoc/credentials/cred_auth.h"
#include "edhoc/credentials/cred_cli_key.h"
#include "edhoc/credentials/cred_pub_data.h"
#include "edhoc/edhoc_config.h"

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

static void cp_cli_try_send_edhoc_error_message(
    struct cp_cli_exchange* exchange,
    const struct com_readonly_buffer error_payload_data) {
  if (exchange == NULL || !com_readonly_buffer_is_valid(error_payload_data)) {
    return;
  }

  const struct cp_cli_exchange_request request_data = {
      .buffer = error_payload_data,
      .content_format = CP_CFG_CONTENT_CID_EDHOC,
  };
  cp_cli_exchange_reset(exchange);
  (void)cp_cli_exchange_send(exchange, request_data);
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
  const struct cp_com_create_context_result create_context_result =
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

  // TODO: create a struct that allows resetting of buffer and handles this
  uint8_t request_payload[CP_CFG_MAX_PDU_SIZE] = {0};
  struct com_writable_buffer request_output = {
      .bytes = request_payload,
      .capacity = CP_CFG_MAX_PDU_SIZE,
      .length = 0,
  };
  const struct edh_cli_message_1_compose_result message_1_result =
      edh_cli_handshake_compose_message_1(&client_resources.handshake,
                                          &request_output);
  if (message_1_result.status != EDH_CLI_MSG1_COMPOSE_OK) {
    coap_log_err("Failed to compose EDHOC message 1\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  struct com_readonly_conversion_result message_1_conversion_result =
      com_writable_as_readonly(request_output);
  if (message_1_conversion_result.status != COM_RDONLY_CONV_OK) {
    coap_log_err("Failed to convert composed message 1 to readonly buffer\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  struct cp_cli_exchange_request message_1_request_data = {
      .buffer = message_1_conversion_result.buffer,
      .content_format = CP_CFG_CONTENT_CID_EDHOC,
  };

  if (cp_cli_exchange_send(&client_resources.exchange,
                           message_1_request_data) != CP_STATUS_SUCCESS) {
    coap_log_err("Failed to send CoAP request for message 1\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  uint8_t response_payload[CP_CFG_MAX_PDU_SIZE] = {0};
  struct com_writable_buffer response_data = {
      .bytes = response_payload,
      .capacity = CP_CFG_MAX_PDU_SIZE,
      .length = 0,
  };
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
  // TODO: resetting this:
  {
    memset(request_payload, 0, sizeof(request_payload));
    request_output.length = 0;
  }
  const struct edh_cli_message_2_process_result message_2_result =
      edh_cli_handshake_process_message_2(&client_resources.handshake,
                                          message_2_input, &request_output);

  if (message_2_result.status != EDH_CLI_MSG2_PROCESS_OK) {
    coap_log_err("Failed to receive or process EDHOC message 2\n");
    struct com_readonly_conversion_result message_2_conversion_result =
        com_writable_as_readonly(request_output);
    // TODO: this is duplicated
    if (message_2_conversion_result.status != COM_RDONLY_CONV_OK) {
      coap_log_err(
          "Failed to convert message 2 composing error to readonly buffer\n");
      cp_cli_cleanup_resources(&client_resources);
      return COM_EMULATION_FAILURE;
    }
    cp_cli_try_send_edhoc_error_message(&client_resources.exchange,
                                        message_2_conversion_result.buffer);
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  cp_cli_exchange_reset(&client_resources.exchange);

  {
    memset(request_payload, 0, sizeof(request_payload));
    request_output.length = 0;
  }
  const struct edh_cli_message_3_compose_result message_3_result =
      edh_cli_handshake_compose_message_3(&client_resources.handshake,
                                          &request_output);
  struct com_readonly_conversion_result message_3_conversion_result =
      com_writable_as_readonly(request_output);
  if (message_3_conversion_result.status != COM_RDONLY_CONV_OK) {
    coap_log_err("Failed to convert composed message 3 to readonly buffer\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  if (message_3_result.status != EDH_CLI_MSG3_COMPOSE_OK) {
    coap_log_err("Failed to compose EDHOC message 3\n");
    cp_cli_try_send_edhoc_error_message(&client_resources.exchange,
                                        message_3_conversion_result.buffer);
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  struct cp_cli_exchange_request message_3_request_data = {
      .buffer = message_3_conversion_result.buffer,
      .content_format = CP_CFG_CONTENT_CID_EDHOC,
  };
  if (cp_cli_exchange_send(&client_resources.exchange,
                           message_3_request_data) != CP_STATUS_SUCCESS) {
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
  const struct edh_cli_message_4_process_result message_4_result =
      edh_cli_handshake_process_message_4(&client_resources.handshake,
                                          message_4_input, &request_output);

  if (message_4_result.status != EDH_CLI_MSG4_PROCESS_OK) {
    coap_log_err("Failed to process EDHOC message 4\n");
    struct com_readonly_conversion_result message_4_conversion_result =
        com_writable_as_readonly(request_output);
    // TODO: this is duplicated
    if (message_4_conversion_result.status != COM_RDONLY_CONV_OK) {
      coap_log_err(
          "Failed to convert message 4 composing error to readonly buffer\n");
      cp_cli_cleanup_resources(&client_resources);
      return COM_EMULATION_FAILURE;
    }
    cp_cli_try_send_edhoc_error_message(&client_resources.exchange,
                                        message_4_conversion_result.buffer);
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  cp_cli_cleanup_resources(&client_resources);
  return COM_EMULATION_SUCCESS;
}