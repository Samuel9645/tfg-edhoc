#include "app/app_client.h"

#include <coap3/coap.h>

#include "coap/client/cli_exchange.h"
#include "coap/client/cli_resources.h"
#include "coap/client/cli_utils.h"
#include "coap/coap_config.h"
#include "coap/common/com_coap_context.h"
#include "common/com_emulation.h"
#include "edhoc/client/handshake/cli_negotiate_cipher_suites.h"
#include "edhoc/client/handshake/message_1/cli_m1_compose.h"
#include "edhoc/client/handshake/message_2/cli_m2_initiator.h"
#include "edhoc/client/handshake/message_4/cli_m4_process.h"
#include "edhoc/common/com_edhoc_setup_context.h"
#include "edhoc/credentials/cred_auth.h"
#include "edhoc/credentials/cred_cli_key.h"
#include "edhoc/credentials/cred_pub_data.h"
#include "edhoc/edhoc_config.h"

static int client_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  return cred_edhoc_auth_fetch(
      user_context, credentials, CRED_EDHOC_PUB_CLI_PK,
      CRED_EDHOC_PUB_PK_LENGTH, CRED_EDHOC_CLI_PRIVATE_KEY,
      CRED_EDHOC_CLI_PRIVATE_KEY_LENGTH, CRED_EDHOC_PUB_CLI_KID);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef (libedhoc signature
// forces it)
static int client_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  return cred_edhoc_auth_verify(
      user_context, credentials, CRED_EDHOC_PUB_SRV_KID, CRED_EDHOC_PUB_SRV_PK,
      CRED_EDHOC_PUB_PK_LENGTH, public_key_reference, public_key_length);
}

static const struct edhoc_credentials credentials = {
    .fetch = client_credential_fetch,
    .verify = client_credential_verify,
};

static bool send_message(struct cli_coap_exchange* exchange,
                         const struct com_readonly_buffer payload) {
  if (exchange == NULL) {
    coap_log_err("Cannot send EDHOC message: exchange is NULL\n");
    return false;
  }
  if (!com_readonly_buffer_has_content(payload)) {
    coap_log_err("Cannot send EDHOC message: payload data is empty\n");
    return false;
  }

  const struct cli_coap_exchange_request request_data = {
      .buffer = payload,
      .content_format = CONFIG_COAP_CONTENT_CID_EDHOC,
  };
  if (cli_coap_exchange_send(exchange, request_data) != STATUS_COAP_OK) {
    coap_log_err("Failed to send CoAP request for EDHOC message\n");
    return false;
  }
  return true;
}

enum com_emulation_status core_run_client(void) {
  coap_startup();
  coap_set_log_level(COAP_LOG_DEBUG);

  static const char CLIENT_COAP_URI[] =
      "coap://localhost:5683/.well-known/edhoc";
  struct cli_coap_parse_and_resolve_result parse_and_resolve_uri_result =
      cli_coap_parse_and_resolve_coap_uri(CLIENT_COAP_URI);
  if (parse_and_resolve_uri_result.status != CLI_COAP_PARSE_AND_RESOLVE_OK) {
    return COM_EMULATION_FAILURE;
  }
  const struct cli_coap_session_config config = {
      .address = &parse_and_resolve_uri_result.address,
      .uri = &parse_and_resolve_uri_result.uri,
  };
  const struct com_coap_create_context_result create_context_result =
      com_coap_create_context();
  if (create_context_result.status != COM_COAP_INIT_OK) {
    return COM_EMULATION_FAILURE;
  }
  const struct cli_coap_create_session_result create_session_result =
      cli_coap_create_session(create_context_result.context, config);
  if (create_session_result.status != CLI_COAP_CREATE_SESSION_OK) {
    return COM_EMULATION_FAILURE;
  }
  struct cli_resources* client_resources = cli_resources_create(
      create_context_result.context, create_session_result.session,
      parse_and_resolve_uri_result.uri, parse_and_resolve_uri_result.address);
  if (client_resources == NULL) {
    coap_log_err("Failed to create client resources\n");
  }

  const enum edhoc_method SUPPORTED_METHODS[] = {EDHOC_METHOD_0};
  const struct srv_edhoc_parameters edhoc_parameters = {
      .credentials = &credentials,
      .supported_cipher_suites = COM_EDHOC_ONLY_SUITE_2,
      .preferred_cipher_suites = COM_EDHOC_ONLY_SUITE_2,
      .methods =
          {
              .data = SUPPORTED_METHODS,
              .size = sizeof(SUPPORTED_METHODS) / sizeof(SUPPORTED_METHODS[0]),
          },
  };
  if (!cli_resources_setup_session(client_resources, edhoc_parameters)) {
    coap_log_err("Failed to setup EDHOC session\n");
    cli_coap_cleanup_resources(client_resources);
    return COM_EMULATION_FAILURE;
  }
  const struct com_writable_buffer payload_buffer =
      cli_resources_get_payload(client_resources);

  struct edhoc_context* edhoc_context =
      cli_resources_get_edhoc_context(client_resources);
  if (edhoc_context == NULL) {
    coap_log_err("Failed to get EDHOC context\n");
    cli_coap_cleanup_resources(client_resources);
    return COM_EMULATION_FAILURE;
  }
  const struct cli_edhoc_message_1_compose_result message_1_result =
      cli_edhoc_compose_message_1(edhoc_context, payload_buffer);
  if (message_1_result.status != CLI_EDHOC_MSG1_COMPOSE_OK) {
    coap_log_err("Failed to compose EDHOC message 1\n");
    cli_coap_cleanup_resources(client_resources);
    return COM_EMULATION_FAILURE;
  }

  struct cli_coap_exchange* exchange =
      cli_resources_get_exchange(client_resources);
  if (exchange == NULL) {
    coap_log_err("Failed to get CoAP exchange for sending message 1\n");
    cli_coap_cleanup_resources(client_resources);
    return COM_EMULATION_FAILURE;
  }
  if (!send_message(exchange, message_1_result.buffer)) {
    coap_log_err("Failed to send CoAP request for message 1\n");
    cli_coap_cleanup_resources(client_resources);
    return COM_EMULATION_FAILURE;
  }
  const struct cli_coap_wait_and_get_result wait_and_get_result =
      cli_coap_exchange_wait_and_get(exchange);
  if (wait_and_get_result.status != STATUS_COAP_OK) {
    coap_log_err("Failed to receive EDHOC message 2\n");
    cli_coap_cleanup_resources(client_resources);
    return COM_EMULATION_FAILURE;
  }

  const struct cli_edhoc_message_2_initiator_request
      message_2_initiator_request = {
          .raw_payload = wait_and_get_result.response,
          .edhoc_context = edhoc_context,
      };
  const struct cli_edhoc_message_2_initiator_result message_2_initiator_result =
      cli_edhoc_respond_to_message_2(message_2_initiator_request,
                                     payload_buffer);

  if (message_2_initiator_result.status != CLI_EDHOC_MSG2_INITIATOR_OK) {
    coap_log_err("%s", cli_edhoc_respond_to_message_2_status_code_to_string(
                           message_2_initiator_result.status));
    send_message(exchange, message_2_initiator_result.request);
    cli_coap_cleanup_resources(client_resources);
    return COM_EMULATION_FAILURE;
  }
  if (!send_message(exchange, message_2_initiator_result.request)) {
    coap_log_err("Failed to send EDHOC message 3\n");
    cli_coap_cleanup_resources(client_resources);
    return COM_EMULATION_FAILURE;
  }

  const struct cli_coap_wait_and_get_result wait_and_get_result2 =
      cli_coap_exchange_wait_and_get(exchange);
  if (wait_and_get_result2.status != STATUS_COAP_OK) {
    coap_log_err("Failed to receive EDHOC message 4\n");
    cli_coap_cleanup_resources(client_resources);
    return COM_EMULATION_FAILURE;
  }
  const struct cli_edhoc_message_4_process_result message_4_result =
      cli_edhoc_process_message_4(edhoc_context, wait_and_get_result2.response,
                                  payload_buffer);
  if (message_4_result.status != CLI_EDHOC_MSG4_PROCESS_OK) {
    coap_log_err("Failed to process EDHOC message 4\n");
    send_message(exchange, message_4_result.error_buffer);
    cli_coap_cleanup_resources(client_resources);
    return COM_EMULATION_FAILURE;
  }
  cli_coap_cleanup_resources(client_resources);
  return COM_EMULATION_SUCCESS;
}