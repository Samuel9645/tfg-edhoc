#include "core/client.h"

#include <coap3/coap.h>

#include "coap/client/cleanup.h"
#include "coap/client/exchange.h"
#include "coap/client/utils.h"
#include "coap/config.h"
#include "edhoc/client/handshake.h"
#include "edhoc/config.h"

// TODO: SEND ERROR RESPONSES TO SERVER IN CASE OF FAILURE INSTEAD OF JUST
// FAILING SILENTLY

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

  if (cp_cli_exchange_init(&exchange_session_data,
                           &client_resources.exchange) != CP_STATUS_SUCCESS) {
    coap_log_err("Failed to initialize CoAP exchange\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  if (edh_cli_handshake_init(&client_resources.handshake) !=
      EDH_CLI_HANDSHAKE_SUCCESS) {
    coap_log_err("Failed to initialize EDHOC handshake\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  uint8_t request_payload[EDH_CFG_MESSAGE_BUFFER_LENGTH] = {0};
  uint8_t response_payload[CP_CFG_MAX_PDU_SIZE] = {0};

  cp_cli_exchange_request_data_t request_data = {
      .request_data =
          {
              .buffer = request_payload,
              .length = 0,
          },
      .content_format = CP_CFG_CONTENT_CID_EDHOC,
  };

  com_response_buffer_t response_data = {
      .buffer = response_payload,
      .capacity = CP_CFG_MAX_PDU_SIZE,
      .length = 0,
  };

  if (edh_cli_handshake_compose_message_1(
          &client_resources.handshake, EDH_CFG_MESSAGE_BUFFER_LENGTH,
          request_payload, &request_data.request_data.length) !=
      EDH_CLI_HANDSHAKE_SUCCESS) {
    coap_log_err("Failed to compose EDHOC message 1\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  if (cp_cli_exchange_send(&client_resources.exchange, &request_data) !=
      CP_STATUS_SUCCESS) {
    coap_log_err("Failed to send CoAP request for message 1\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  if (cp_cli_exchange_wait_and_get(&client_resources.exchange,
                                   &response_data) != CP_STATUS_SUCCESS ||
      edh_cli_handshake_process_message_2(
          &client_resources.handshake, response_payload,
          response_data.length) != EDH_CLI_HANDSHAKE_SUCCESS) {
    coap_log_err("Failed to receive or process EDHOC message 2\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  cp_cli_exchange_reset(&client_resources.exchange);

  if (edh_cli_handshake_compose_message_3(
          &client_resources.handshake, EDH_CFG_MESSAGE_BUFFER_LENGTH,
          request_payload, &request_data.request_data.length) !=
      EDH_CLI_HANDSHAKE_SUCCESS) {
    coap_log_err("Failed to compose EDHOC message 3\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  if (cp_cli_exchange_send(&client_resources.exchange, &request_data) !=
          CP_STATUS_SUCCESS ||
      cp_cli_exchange_wait_and_get(&client_resources.exchange,
                                   &response_data) != CP_STATUS_SUCCESS ||
      edh_cli_handshake_process_message_4(
          &client_resources.handshake, response_payload,
          response_data.length) != EDH_CLI_HANDSHAKE_SUCCESS) {
    coap_log_err(
        "Failed to complete EDHOC message 3 exchange or process message 4\n");
    cp_cli_cleanup_resources(&client_resources);
    return COM_EMULATION_FAILURE;
  }

  cp_cli_cleanup_resources(&client_resources);
  return COM_EMULATION_SUCCESS;
}