#include "app/app_client.h"

#include <coap3/coap.h>
#include <string.h>

#include "coap/client/cli_exchange.h"
#include "coap/client/cli_resources.h"
#include "coap/client/cli_utils.h"
#include "coap/coap_config.h"
#include "coap/common/com_coap_context.h"
#include "common/com_emulation.h"
#include "common/com_session_resources.h"
#include "edhoc/client/cli_handshake.h"
#include "edhoc/credentials/cred_auth.h"
#include "edhoc/credentials/cred_cli_key.h"
#include "edhoc/credentials/cred_pub_data.h"
#include "edhoc/edhoc_config.h"

static int client_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  return cred_edhoc_auth_fetch(
      user_context, credentials, CRED_EDHOC_PUB_CLI_PK,
      ARRAY_SIZE(CRED_EDHOC_PUB_CLI_PK), CRED_EDHOC_CLI_PRIVATE_KEY,
      ARRAY_SIZE(CRED_EDHOC_CLI_PRIVATE_KEY), CRED_EDHOC_PUB_CLI_KID);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef (libedhoc signature
// forces it)
static int client_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  return cred_edhoc_auth_verify(user_context, credentials,
                                CRED_EDHOC_PUB_SRV_KID, CRED_EDHOC_PUB_SRV_PK,
                                ARRAY_SIZE(CRED_EDHOC_PUB_SRV_PK),
                                public_key_reference, public_key_length);
}

static const struct edhoc_credentials credentials = {
    .fetch = client_credential_fetch,
    .verify = client_credential_verify,
};

static void cli_coap_try_send_edhoc_error_message(
    struct cli_coap_exchange* exchange,
    const struct com_readonly_buffer error_payload_data) {
  if (exchange == NULL ||
      !com_readonly_buffer_has_content(error_payload_data)) {
    return;
  }

  const struct cli_coap_exchange_request request_data = {
      .buffer = error_payload_data,
      .content_format = CONFIG_COAP_CONTENT_CID_EDHOC,
  };
  cli_coap_exchange_reset(exchange);
  (void)cli_coap_exchange_send(exchange, request_data);
}

enum com_emulation_status core_run_client(void) {
  coap_startup();
  coap_set_log_level(COAP_LOG_DEBUG);

  struct cli_coap_session_resources client_resources = {0};
  static const char CLIENT_COAP_URI[] =
      "coap://localhost:5683/.well-known/edhoc";
  struct cli_coap_parse_and_resolve_result parse_and_resolve_uri_result =
      cli_coap_parse_and_resolve_coap_uri(CLIENT_COAP_URI);
  if (parse_and_resolve_uri_result.status != CLI_COAP_PARSE_AND_RESOLVE_OK) {
    return COM_EMULATION_FAILURE;
  }
  struct cli_coap_session_config config = {
      .address = &parse_and_resolve_uri_result.address,
      .uri = &parse_and_resolve_uri_result.uri,
  };
  const struct com_coap_create_context_result create_context_result =
      com_coap_create_context();
  if (create_context_result.status != COM_COAP_INIT_OK) {
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  client_resources.common_resources.coap_context =
      create_context_result.context;
  struct cli_coap_create_session_result create_session_result =
      cli_coap_create_session(client_resources.common_resources.coap_context,
                              config);
  if (create_session_result.status != CLI_COAP_CREATE_SESSION_OK) {
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  client_resources.coap_session = create_session_result.session;
  struct cli_coap_exchange_session_data exchange_session_data = {
      .context = client_resources.common_resources.coap_context,
      .session = client_resources.coap_session,
      .uri = parse_and_resolve_uri_result.uri,
      .destination = parse_and_resolve_uri_result.address,
  };
  if (cli_coap_init_exchange(&exchange_session_data,
                             &client_resources.exchange) != STATUS_COAP_OK) {
    coap_log_err("Failed to initialize CoAP exchange\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  if (cli_edhoc_init_handshake(&client_resources.handshake, &credentials) !=
      CLI_EDHOC_INIT_OK) {
    coap_log_err("Failed to initialize EDHOC handshake\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  // TODO: create a struct that allows resetting of buffer and handles this
  uint8_t request_payload[CONFIG_COAP_MAX_PDU_SIZE] = {0};
  struct com_writable_buffer request_output = {
      .bytes = request_payload,
      .capacity = CONFIG_COAP_MAX_PDU_SIZE,
      .length = 0,
  };
  const struct cli_edhoc_message_1_compose_result message_1_result =
      cli_edhoc_handshake_compose_message_1(&client_resources.handshake,
                                            &request_output);
  if (message_1_result.status != CLI_EDHOC_MSG1_COMPOSE_OK) {
    coap_log_err("Failed to compose EDHOC message 1\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  struct com_readonly_conversion_result message_1_conversion_result =
      com_writable_as_readonly(&request_output);
  if (message_1_conversion_result.status != COM_RDONLY_CONV_OK) {
    coap_log_err("Failed to convert composed message 1 to readonly buffer\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  struct cli_coap_exchange_request message_1_request_data = {
      .buffer = message_1_conversion_result.buffer,
      .content_format = CONFIG_COAP_CONTENT_CID_EDHOC,
  };

  if (cli_coap_exchange_send(&client_resources.exchange,
                             message_1_request_data) != STATUS_COAP_OK) {
    coap_log_err("Failed to send CoAP request for message 1\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  uint8_t response_payload[CONFIG_COAP_MAX_PDU_SIZE] = {0};
  struct com_writable_buffer response_data = {
      .bytes = response_payload,
      .capacity = CONFIG_COAP_MAX_PDU_SIZE,
      .length = 0,
  };
  if (cli_coap_exchange_wait_and_get(&client_resources.exchange,
                                     &response_data) != STATUS_COAP_OK) {
    coap_log_err("Failed to receive EDHOC message 2\n");
    cli_coap_cleanup_resources(&client_resources);
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
  const struct cli_edhoc_message_2_process_result message_2_result =
      cli_edhoc_handshake_process_message_2(&client_resources.handshake,
                                            message_2_input, &request_output);

  if (message_2_result.status != CLI_EDHOC_MSG2_PROCESS_OK) {
    coap_log_err("Failed to receive or process EDHOC message 2\n");
    struct com_readonly_conversion_result message_2_conversion_result =
        com_writable_as_readonly(&request_output);
    // TODO: this is duplicated
    if (message_2_conversion_result.status != COM_RDONLY_CONV_OK) {
      coap_log_err(
          "Failed to convert message 2 composing error to readonly buffer\n");
      cli_coap_cleanup_resources(&client_resources);
      return COM_EMULATION_FAILURE;
    }
    cli_coap_try_send_edhoc_error_message(&client_resources.exchange,
                                          message_2_conversion_result.buffer);
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  cli_coap_exchange_reset(&client_resources.exchange);

  {
    memset(request_payload, 0, sizeof(request_payload));
    request_output.length = 0;
  }
  const struct cli_edhoc_message_3_compose_result message_3_result =
      cli_edhoc_handshake_compose_message_3(&client_resources.handshake,
                                            &request_output);
  struct com_readonly_conversion_result message_3_conversion_result =
      com_writable_as_readonly(&request_output);
  if (message_3_conversion_result.status != COM_RDONLY_CONV_OK) {
    coap_log_err("Failed to convert composed message 3 to readonly buffer\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  if (message_3_result.status != CLI_EDHOC_MSG3_COMPOSE_OK) {
    coap_log_err("Failed to compose EDHOC message 3\n");
    cli_coap_try_send_edhoc_error_message(&client_resources.exchange,
                                          message_3_conversion_result.buffer);
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  struct cli_coap_exchange_request message_3_request_data = {
      .buffer = message_3_conversion_result.buffer,
      .content_format = CONFIG_COAP_CONTENT_CID_EDHOC,
  };
  if (cli_coap_exchange_send(&client_resources.exchange,
                             message_3_request_data) != STATUS_COAP_OK) {
    coap_log_err("Failed to send EDHOC message 3\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  if (cli_coap_exchange_wait_and_get(&client_resources.exchange,
                                     &response_data) != STATUS_COAP_OK) {
    coap_log_err("Failed to receive EDHOC message 4\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  const struct com_readonly_buffer message_4_input = {
      .bytes = response_payload,
      .length = response_data.length,
  };
  request_output.length = 0;
  const struct cli_edhoc_message_4_process_result message_4_result =
      cli_edhoc_handshake_process_message_4(&client_resources.handshake,
                                            message_4_input, &request_output);

  if (message_4_result.status != CLI_EDHOC_MSG4_PROCESS_OK) {
    coap_log_err("Failed to process EDHOC message 4\n");
    struct com_readonly_conversion_result message_4_conversion_result =
        com_writable_as_readonly(&request_output);
    // TODO: this is duplicated
    if (message_4_conversion_result.status != COM_RDONLY_CONV_OK) {
      coap_log_err(
          "Failed to convert message 4 composing error to readonly buffer\n");
      cli_coap_cleanup_resources(&client_resources);
      return COM_EMULATION_FAILURE;
    }
    cli_coap_try_send_edhoc_error_message(&client_resources.exchange,
                                          message_4_conversion_result.buffer);
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  cli_coap_cleanup_resources(&client_resources);
  return COM_EMULATION_SUCCESS;
}