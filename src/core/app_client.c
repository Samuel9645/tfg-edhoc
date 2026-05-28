#include "app/app_client.h"

#include <coap3/coap.h>
#include <stdio.h>

#include "coap/client/cli_exchange.h"
#include "coap/client/cli_resources.h"
#include "coap/client/cli_utils.h"
#include "coap/coap_config.h"
#include "coap/common/com_coap_context.h"
#include "coap/common/com_coap_get_data.h"
#include "common/com_emulation.h"
#include "common/com_logging.h"
#include "edhoc/client/handshake/cli_negotiate_cipher_suites.h"
#include "edhoc/client/handshake/message_1/cli_m1_compose.h"
#include "edhoc/client/handshake/message_2/cli_m2_initiator.h"
#include "oscore/client/cli_oscore_create_session.h"

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

static bool response_received = false;
static bool error_response = false;

static coap_response_t client_temperature_handler(
    // ReSharper disable once CppParameterMayBeConstPtrOrRef
    coap_session_t* session, const coap_pdu_t* sent, const coap_pdu_t* received,
    const coap_mid_t id) {
  (void)session;
  (void)sent;
  (void)id;
  coap_log_info("Response received from server!\n");
  response_received = true;
  const coap_pdu_code_t pdu_code = coap_pdu_get_code(received);
  if (pdu_code != COAP_RESPONSE_CODE_CONTENT) {
    coap_log_err("Received error response from server with PDU: ");
    error_response = true;
    coap_show_pdu(COAP_LOG_ERR, received);
    return COAP_RESPONSE_OK;
  }
  uint8_t response_buffer[CONFIG_COAP_MAX_PDU_SIZE] = {0};
  const struct com_writable_buffer response_data = {
      .bytes = response_buffer, .capacity = sizeof(response_buffer)};
  const struct com_coap_get_data_result get_data_result =
      com_coap_get_data(received, response_data);
  if (get_data_result.status != COM_COAP_GET_DATA_OK) {
    coap_log_err("Failed to get response data\n");
    return COAP_RESPONSE_OK;
  }
  if (!com_readonly_buffer_has_content(get_data_result.data)) {
    coap_log_err("Response data is empty\n");
    return COAP_RESPONSE_OK;
  }
  coap_log_info("Received response data: %.*s\n",
                (int)get_data_result.data.length,
                (const char*)get_data_result.data.bytes);
  return COAP_RESPONSE_OK;
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
    struct cli_resources* resources,
    const struct com_writable_buffer payload_buffer) {
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
  const struct cli_coap_wait_and_get_result message_1_or_error_response =
      cli_coap_exchange_wait_and_get(exchange);
  if (message_1_or_error_response.status != STATUS_COAP_OK) {
    coap_log_err("Failed get Message 1 response\n");
    return failure();
  }
  if (message_1_or_error_response.response_is_error &&
      cli_edhoc_error_suggests_renegotiation(
          message_1_or_error_response.response)) {
    coap_log_info(
        "Received error response to Message 1, attempting renegotiation\n");
    return renegotiation(message_1_or_error_response);
  }
  coap_log_info("Received response to Message 1, proceeding with handshake\n");
  return ok(message_1_or_error_response);
}

static struct cli_edhoc_negotiation_attempt_result
cli_edhoc_resolve_negotiation(
    struct cli_resources* client_resources,
    const struct com_edhoc_parameters initial_edhoc_parameters,
    const struct com_edhoc_cipher_suite_list preferred_suites,
    const struct com_writable_buffer payload_buffer) {
  const struct cli_edhoc_negotiation_attempt_result result =
      cli_edhoc_perform_negotiation_attempt(client_resources, payload_buffer);
  if (result.status == CLI_EDHOC_NEGOTIATION_ERR) {
    coap_log_err("Failed to perform initial EDHOC M1-M2 exchange\n");
    return result;
  }
  if (result.status != CLI_EDHOC_NEGOTIATION_RENEGOTIATE) {
    return result;
  }
  const struct cli_edhoc_suites_negotiation_result negotiation_result =
      cli_edhoc_negotiate_suites(
          initial_edhoc_parameters.supported_cipher_suites, preferred_suites,
          result.response_payload);
  if (negotiation_result.status != CLI_EDHOC_NEGOTIATE_SUITES_OK) {
    return failure();
  }
  if (!cli_reset_edhoc_context_with_new_suites_data(
          client_resources, initial_edhoc_parameters,
          negotiation_result.selected_suite,
          negotiation_result.renegotiation_suites)) {
    coap_log_err("Failed to reset EDHOC context with new suites data\n");
    return failure();
  }

  return cli_edhoc_perform_negotiation_attempt(client_resources,
                                               payload_buffer);
}

static bool build_server_uri(char* destination_buffer, const size_t buffer_size,
                             const char* server_ip, const char* resource_path) {
  const int bytes_written =
      snprintf(destination_buffer, buffer_size, "coap://%s:5683/%s", server_ip,
               resource_path);

  if (bytes_written < 0 || (size_t)bytes_written >= buffer_size) {
    com_log_error("Error: Server IP address or URI path '%s' is too long.\n",
                  resource_path);
    return false;
  }
  return true;
}

enum com_emulation_status core_run_client(
    const struct com_edhoc_parameters edhoc_parameters,
    const struct com_edhoc_cipher_suite_list preferred_suites,
    const char* server_ip) {
  coap_startup();
  coap_set_log_level(COAP_LOG_INFO);

  enum { SERVER_URI_BUFFER_SIZE = 256 };

  char server_edhoc_uri[SERVER_URI_BUFFER_SIZE] = {0};
  char server_oscore_resource_uri[SERVER_URI_BUFFER_SIZE] = {0};
  if (!build_server_uri(server_edhoc_uri, sizeof(server_edhoc_uri), server_ip,
                        ".well-known/edhoc") ||
      !build_server_uri(server_oscore_resource_uri,
                        sizeof(server_oscore_resource_uri), server_ip,
                        "sensors/temperature")) {
    return COM_EMULATION_FAILURE;
  }
  struct cli_coap_parse_and_resolve_result parse_and_resolve_uri_result =
      cli_coap_parse_and_resolve_coap_uri(server_edhoc_uri);
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
  struct cli_resources client_resources = {
      .coap_context = create_context_result.context,
      .exchange_session = create_session_result.session,
  };

  if (!cli_initialize_edhoc_context_with_parameters(&client_resources,
                                                    edhoc_parameters)) {
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

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
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  const struct cli_edhoc_negotiation_attempt_result
      message_1_negotiation_attempt_result =
          cli_edhoc_resolve_negotiation(&client_resources, edhoc_parameters,
                                        preferred_suites, payload_buffer);

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
  coap_log_info(
      "Message 2 process and Message 3 compose completed successfully\n");
  struct com_readonly_buffer message_3 = message_2_initiator_result.buffer;
  if (cli_exchange_send_message_3(exchange, &client_resources.edhoc_context,
                                  message_3) != STATUS_COAP_OK) {
    coap_log_err("Failed to send EDHOC message 3\n");
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  const struct cli_coap_wait_and_get_result message_3_or_error_response =
      cli_coap_exchange_wait_and_get(exchange);
  if (message_3_or_error_response.status != STATUS_COAP_OK) {
    cli_cleanup_resources(&client_resources);
    coap_log_err("Failed to get Message 3 response\n");
    return COM_EMULATION_FAILURE;
  }
  if (message_3_or_error_response.response_is_error) {
    coap_log_err("Received error response to Message 3");
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  coap_log_info("Handshake finished successfully. Activating OSCORE...\n");

  struct cli_coap_parse_and_resolve_result parse_temp_uri_result =
      cli_coap_parse_and_resolve_coap_uri(server_oscore_resource_uri);
  if (parse_temp_uri_result.status != CLI_COAP_PARSE_AND_RESOLVE_OK) {
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  client_resources.oscore_session = cli_oscore_create_session(
      create_context_result.context, &client_resources.edhoc_context,
      &parse_temp_uri_result.address);

  if (client_resources.oscore_session == NULL) {
    coap_log_err("Client failed to establish OSCORE session context\n");
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }
  coap_register_response_handler(create_context_result.context,
                                 client_temperature_handler);

  coap_log_info("Sending encrypted GET to /sensors/temperature\n");

  struct cli_coap_session_config endpoint_configuration = {
      .uri = &parse_temp_uri_result.uri,
      .address = &parse_temp_uri_result.address};
  struct cli_coap_prepare_pdu_result prepare_result =
      cli_coap_prepare_get_request(endpoint_configuration,
                                   client_resources.oscore_session);

  if (prepare_result.status != CLI_COAP_PREPARE_PDU_OK) {
    coap_session_release(client_resources.oscore_session);
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  response_received = false;
  if (cli_coap_send_coap_request(client_resources.oscore_session,
                                 prepare_result.pdu) != STATUS_COAP_OK) {
    coap_delete_pdu(prepare_result.pdu);
    coap_session_release(client_resources.oscore_session);
    cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  cli_coap_wait_for_coap_response(create_context_result.context,
                                  client_resources.oscore_session,
                                  &response_received);
  coap_session_release(client_resources.oscore_session);
  cli_cleanup_resources(&client_resources);
  if (error_response) {
    coap_log_err(
        "Key Confirmation Failed: Received error response to encrypted "
        "request.\n");
    return COM_EMULATION_FAILURE;
  }
  if (!response_received) {
    coap_log_err("Key Confirmation Failed: No encrypted response received.\n");
    return COM_EMULATION_FAILURE;
  }
  return COM_EMULATION_SUCCESS;
}