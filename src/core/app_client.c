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

static const struct edhoc_credentials CREDENTIALS = {
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

enum cli_first_interaction_status {
  CLI_EDHOC_NEGOTIATION_OK,
  CLI_EDHOC_NEGOTIATION_RENEGOTIATE,
  CLI_EDHOC_NEGOTIATION_ERR
};

// ReSharper disable once CppClassNeverUsed
struct cli_edhoc_negotiation_attempt_result {
  enum cli_first_interaction_status status;
  struct com_readonly_buffer response_payload;
};

static struct cli_edhoc_negotiation_attempt_result failure(void) {
  return (struct cli_edhoc_negotiation_attempt_result){
      .status = CLI_EDHOC_NEGOTIATION_ERR};
}

static struct cli_edhoc_negotiation_attempt_result ok(
    const struct cli_coap_wait_and_get_result wait_and_get_result) {
  return (struct cli_edhoc_negotiation_attempt_result){
      .status = CLI_EDHOC_NEGOTIATION_OK,
      .response_payload = wait_and_get_result.response};
}

static struct cli_edhoc_negotiation_attempt_result renegotiation(
    const struct cli_coap_wait_and_get_result wait_and_get_result) {
  return (struct cli_edhoc_negotiation_attempt_result){
      .status = CLI_EDHOC_NEGOTIATION_RENEGOTIATE,
      .response_payload = wait_and_get_result.response};
}

static struct cli_edhoc_negotiation_attempt_result
cli_edhoc_perform_negotiation_attempt(
    struct cli_resources* resources, const struct com_edhoc_parameters params,
    const struct com_writable_buffer payload_buffer) {
  if (com_edhoc_setup_context(&resources->edhoc_context, params,
                              cli_resources_get_payload(resources))
          .status != COM_EDHOC_SETUP_CTX_OK) {
    coap_log_err("Failed to initialize EDHOC context\n");
    return failure();
  }

  const struct cli_edhoc_message_1_compose_result compose_result =
      cli_edhoc_compose_message_1(&resources->edhoc_context, payload_buffer);
  if (compose_result.status != CLI_EDHOC_MSG1_COMPOSE_OK) {
    coap_log_err("Failed to compose Message 1\n");
    return failure();
  }
  struct cli_coap_exchange* exchange = resources->exchange;
  if (cli_exchange_send_message_1(exchange, compose_result.buffer) !=
      STATUS_COAP_OK) {
    return failure();
  }
  const struct cli_coap_wait_and_get_result wait_and_get_result =
      cli_coap_exchange_wait_and_get(exchange);
  if (wait_and_get_result.status != STATUS_COAP_OK) {
    coap_log_err("Failed get Message 1 response\n");
    return failure();
  }
  if (wait_and_get_result.response_is_error &&
      cli_edhoc_error_suggests_renegotiation(wait_and_get_result.response)) {
    coap_log_info(
        "Received error response to Message 1, attempting renegotiation\n");
    return renegotiation(wait_and_get_result);
  }
  return ok(wait_and_get_result);
}

static struct cli_edhoc_negotiation_attempt_result
cli_edhoc_resolve_negotiation(
    struct cli_resources* client_resources,
    const struct com_edhoc_parameters edhoc_parameters,
    const struct com_edhoc_cipher_suite_list supported_suites,
    const struct com_edhoc_cipher_suite_list initial_preferred_suites,
    const struct com_writable_buffer payload_buffer) {
  const struct cli_edhoc_negotiation_attempt_result result =
      cli_edhoc_perform_negotiation_attempt(client_resources, edhoc_parameters,
                                            payload_buffer);
  if (result.status == CLI_EDHOC_NEGOTIATION_ERR) {
    coap_log_err("Failed to perform initial EDHOC M1-M2 exchange\n");
    return result;
  }
  if (result.status != CLI_EDHOC_NEGOTIATION_RENEGOTIATE) {
    return result;
  }
  struct cli_edhoc_suites_negotiation_result negotiation_result =
      cli_edhoc_negotiate_suites(supported_suites, initial_preferred_suites,
                                 result.response_payload);
  if (negotiation_result.status != CLI_EDHOC_NEGOTIATE_SUITES_OK) {
    return failure();
  }
  const struct com_edhoc_parameters retry_params = {
      .credentials = edhoc_parameters.credentials,
      .methods = edhoc_parameters.methods,
      .supported_cipher_suites =
          {
              .number_of_suites =
                  negotiation_result.renegotiation_suites.number_of_suites,
              .suites = negotiation_result.renegotiation_suites.suites,
          },
      .selected_cipher_suite = negotiation_result.selected_suite};
  cli_reset_edhoc_context(client_resources);

  return cli_edhoc_perform_negotiation_attempt(client_resources, retry_params,
                                               payload_buffer);
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

  const enum edhoc_method SUPPORTED_METHODS[] = {EDHOC_METHOD_0};
  const struct com_edhoc_cipher_suite_list SUPPORTED_SUITES =
      COM_EDHOC_SUITES_2_0;
  const struct com_edhoc_cipher_suite_list INITIAL_PREFERRED_SUITES =
      COM_EDHOC_ONLY_SUITE_0;
  const struct com_edhoc_parameters edhoc_parameters = {
      .credentials = &CREDENTIALS,
      .supported_cipher_suites = INITIAL_PREFERRED_SUITES,
      .selected_cipher_suite = INITIAL_PREFERRED_SUITES.suites[0],
      .methods =
          {
              .data = SUPPORTED_METHODS,
              .size = sizeof(SUPPORTED_METHODS) / sizeof(SUPPORTED_METHODS[0]),
          },
  };

  struct cli_resources client_resources = {0};
  const struct com_writable_buffer payload_buffer = {
      .bytes = client_resources.payload, .capacity = CONFIG_COAP_MAX_PDU_SIZE};
  const struct cli_coap_exchange_session_data session_data = {
      .session = create_session_result.session,
      .context = create_context_result.context,
      .destination = parse_and_resolve_uri_result.address,
      .uri = parse_and_resolve_uri_result.uri,
  };
  client_resources.exchange =
      cli_coap_init_exchange(&session_data, payload_buffer);
  if (client_resources.exchange == NULL) {
    coap_log_err("Failed to initialize CoAP exchange\n");
    return COM_EMULATION_FAILURE;
  }

  const struct cli_edhoc_negotiation_attempt_result
      message_1_negotiation_attempt_result = cli_edhoc_resolve_negotiation(
          &client_resources, edhoc_parameters, SUPPORTED_SUITES,
          INITIAL_PREFERRED_SUITES, payload_buffer);

  if (message_1_negotiation_attempt_result.status != CLI_EDHOC_NEGOTIATION_OK) {
    coap_log_err("Handshake failed after all attempts\n");
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  const struct cli_edhoc_message_2_initiator_request
      message_2_initiator_request = {
          .raw_payload = message_1_negotiation_attempt_result.response_payload,
          .edhoc_context = &client_resources.edhoc_context,
      };
  const struct cli_edhoc_message_2_initiator_result message_2_initiator_result =
      cli_edhoc_respond_to_message_2(message_2_initiator_request,
                                     payload_buffer);

  struct cli_coap_exchange* exchange = client_resources.exchange;
  if (exchange == NULL) {
    coap_log_err("Failed to get exchange\n");
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  if (message_2_initiator_result.status != CLI_EDHOC_MSG2_INITIATOR_OK) {
    coap_log_err("%s", cli_edhoc_respond_to_message_2_status_code_to_string(
                           message_2_initiator_result.status));
    send_message(exchange, message_2_initiator_result.buffer);
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  struct com_readonly_buffer message_3 = message_2_initiator_result.buffer;
  if (cli_exchange_send_message_3(exchange, &client_resources.edhoc_context,
                                  message_3) != STATUS_COAP_OK) {
    coap_log_err("Failed to send EDHOC message 3\n");
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  const struct cli_coap_wait_and_get_result wait_and_get_result2 =
      cli_coap_exchange_wait_and_get(exchange);
  if (wait_and_get_result2.status != STATUS_COAP_OK) {
    coap_log_err("Failed to receive EDHOC message 4\n");
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  if (wait_and_get_result2.response_is_error) {
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  const struct cli_edhoc_message_4_process_result message_4_result =
      cli_edhoc_process_message_4(&client_resources.edhoc_context,
                                  wait_and_get_result2.response,
                                  payload_buffer);
  if (message_4_result.status != CLI_EDHOC_MSG4_PROCESS_OK) {
    coap_log_err("Failed to process EDHOC message 4\n");
    send_message(exchange, message_4_result.error_buffer);
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  cli_cleanup_resources(&client_resources);
  return COM_EMULATION_SUCCESS;
}