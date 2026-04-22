#include "app/app_client.h"

#include <coap3/coap.h>

#include "coap/client/cli_exchange.h"
#include "coap/client/cli_resources.h"
#include "coap/client/cli_utils.h"
#include "coap/coap_config.h"
#include "coap/common/com_coap_context.h"
#include "common/com_emulation.h"
#include "common/com_session_resources.h"
#include "edhoc/client/handshake/message_1/cli_m1_compose.h"
#include "edhoc/client/handshake/message_2/cli_m2_initiator.h"
#include "edhoc/client/handshake/message_4/cli_m4_process.h"
#include "edhoc/common/com_edhoc_context_setup.h"
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
  // TODO: encapsulate this process better
  uint8_t payload_memory[CONFIG_COAP_MAX_PDU_SIZE] = {0};
  struct com_writable_buffer payload_buffer = {
      .bytes = payload_memory,
      .capacity = CONFIG_COAP_MAX_PDU_SIZE,
      .length = 0,
  };
  client_resources.exchange =
      cli_coap_init_exchange(&exchange_session_data, payload_buffer);
  if (client_resources.exchange == NULL) {
    coap_log_err("Failed to initialize CoAP exchange\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  if (com_edhoc_setup_context(&client_resources.edhoc_ctx, &credentials) !=
      COM_EDHOC_SETUP_CTX_OK) {
    coap_log_err("Failed to initialize EDHOC context\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  const struct cli_edhoc_message_1_compose_result message_1_result =
      cli_edhoc_compose_message_1(&client_resources.edhoc_ctx, &payload_buffer);
  if (message_1_result.status != CLI_EDHOC_MSG1_COMPOSE_OK) {
    coap_log_err("Failed to compose EDHOC message 1\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  struct com_readonly_conversion_result message_1_conversion_result =
      com_writable_as_readonly(&payload_buffer);
  if (message_1_conversion_result.status != COM_RDONLY_CONV_OK) {
    coap_log_err("Failed to convert composed message 1 to readonly buffer\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  struct cli_coap_exchange_request message_1_request_data = {
      .buffer = message_1_conversion_result.buffer,
      .content_format = CONFIG_COAP_CONTENT_CID_EDHOC,
  };

  if (cli_coap_exchange_send(client_resources.exchange,
                             message_1_request_data) != STATUS_COAP_OK) {
    coap_log_err("Failed to send CoAP request for message 1\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  struct cli_coap_wait_and_get_result wait_and_get_result =
      cli_coap_exchange_wait_and_get(client_resources.exchange);
  if (wait_and_get_result.status != STATUS_COAP_OK) {
    coap_log_err("Failed to receive EDHOC message 2\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  const struct cli_edhoc_message_2_initiator_result message_3_initiator_result =
      cli_edhoc_respond_to_message_2(
          (struct cli_edhoc_message_2_initiator_request){
              .raw_payload = wait_and_get_result.response,
              .edhoc_context = &client_resources.edhoc_ctx,
          },
          &payload_buffer);

  if (message_3_initiator_result.status != CLI_EDHOC_MSG2_INITIATOR_OK) {
    coap_log_err("Failed to receive or process EDHOC message 2\n");
    cli_coap_try_send_edhoc_error_message(client_resources.exchange,
                                          message_3_initiator_result.request);
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  struct cli_coap_exchange_request message_3_request_data = {
      .buffer = message_3_initiator_result.request,
      .content_format = CONFIG_COAP_CONTENT_CID_EDHOC,
  };
  if (cli_coap_exchange_send(client_resources.exchange,
                             message_3_request_data) != STATUS_COAP_OK) {
    coap_log_err("Failed to send EDHOC message 3\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  struct cli_coap_wait_and_get_result wait_and_get_result2 =
      cli_coap_exchange_wait_and_get(client_resources.exchange);
  if (wait_and_get_result2.status != STATUS_COAP_OK) {
    coap_log_err("Failed to receive EDHOC message 4\n");
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  const struct cli_edhoc_message_4_process_result message_4_result =
      cli_edhoc_process_message_4(&client_resources.edhoc_ctx,
                                  wait_and_get_result2.response,
                                  &payload_buffer);

  if (message_4_result.status != CLI_EDHOC_MSG4_PROCESS_OK) {
    coap_log_err("Failed to process EDHOC message 4\n");
    cli_coap_try_send_edhoc_error_message(client_resources.exchange,
                                          message_4_result.error_buffer);
    cli_coap_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  cli_coap_cleanup_resources(&client_resources);
  return COM_EMULATION_SUCCESS;
}