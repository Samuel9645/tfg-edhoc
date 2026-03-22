#include "client.h"

#include <coap3/coap.h>
#include <stdio.h>

#include "coap/client_edhoc_exchange.h"
#include "coap/client_utils.h"
#include "coap/common/config.h"
#include "common/cleanup.h"
#include "edhoc/client/client_flow.h"
#include "edhoc/common/config.h"

// TODO: SEND ERROR RESPONSES TO SERVER IN CASE OF FAILURE INSTEAD OF JUST
// FAILING SILENTLY

emulation_status_t run_client(void) {
  coap_startup();
  coap_set_log_level(COAP_LOG_WARN);

  session_resources_t client_resources = {0};
  client_edhoc_flow_t flow = {0};
  client_edhoc_exchange_t* exchange = NULL;

  static const char CLIENT_COAP_URI[] =
      "coap://localhost:5683/.well-known/edhoc";
  coap_uri_t client_uri = {0};
  coap_address_t destination_address = {0};
  if (parse_and_resolve_coap_uri(CLIENT_COAP_URI, &client_uri,
                                 &destination_address) != COAP_STATUS_SUCCESS) {
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }
  if (create_coap_client_session(&client_uri, &destination_address, NULL,
                                 &client_resources.coap_context,
                                 &client_resources.coap_session) !=
      COAP_STATUS_SUCCESS) {
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  client_edhoc_exchange_session_data_t exchange_session_data = {
      .session_data =
          {
              .context = client_resources.coap_context,
              .session = client_resources.coap_session,
          },
      .endpoint_data =
          {
              .uri = &client_uri,
              .destination = &destination_address,
          },
  };

  if (client_edhoc_exchange_init(&exchange_session_data, &exchange) !=
      COAP_STATUS_SUCCESS) {
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  if (client_edhoc_flow_init(&flow) != CLIENT_EDHOC_FLOW_SUCCESS) {
    client_edhoc_exchange_deinit(&exchange);
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  uint8_t request_payload[MESSAGE_BUFFER_LENGTH] = {0};
  size_t request_len = 0;
  uint8_t response_payload[MAX_PDU_SIZE] = {0};
  size_t response_len = 0;

  client_edhoc_exchange_request_data_t request_data = {
      .request_data =
          {
              .payload = request_payload,
              .payload_len = 0,
          },
      .content_format = APPLICATION_CID_EDHOC_CBOR_SEQ,
  };

  coap_response_data_t response_data = {
      .payload = response_payload,
      .payload_capacity = MAX_PDU_SIZE,
      .payload_len = &response_len,
  };

  if (client_edhoc_flow_compose_message_1(&flow, MESSAGE_BUFFER_LENGTH,
                                          request_payload, &request_len) !=
      CLIENT_EDHOC_FLOW_SUCCESS) {
    client_edhoc_flow_deinit(&flow);
    client_edhoc_exchange_deinit(&exchange);
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  request_data.request_data.payload_len = request_len;

  if (client_edhoc_exchange_send(exchange, &request_data) !=
      COAP_STATUS_SUCCESS) {
    client_edhoc_flow_deinit(&flow);
    client_edhoc_exchange_deinit(&exchange);
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  if (client_edhoc_exchange_wait_and_get(exchange, &response_data) !=
          COAP_STATUS_SUCCESS ||
      client_edhoc_flow_process_message_2(
          &flow, response_payload, response_len) != CLIENT_EDHOC_FLOW_SUCCESS) {
    client_edhoc_flow_deinit(&flow);
    client_edhoc_exchange_deinit(&exchange);
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  client_edhoc_exchange_reset(exchange);

  if (client_edhoc_flow_compose_message_3(&flow, MESSAGE_BUFFER_LENGTH,
                                          request_payload, &request_len) !=
      CLIENT_EDHOC_FLOW_SUCCESS) {
    client_edhoc_flow_deinit(&flow);
    client_edhoc_exchange_deinit(&exchange);
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  request_data.request_data.payload_len = request_len;

  if (client_edhoc_exchange_send(exchange, &request_data) !=
          COAP_STATUS_SUCCESS ||
      client_edhoc_exchange_wait_and_get(exchange, &response_data) !=
          COAP_STATUS_SUCCESS ||
      client_edhoc_flow_process_message_4(
          &flow, response_payload, response_len) != CLIENT_EDHOC_FLOW_SUCCESS) {
    client_edhoc_flow_deinit(&flow);
    client_edhoc_exchange_deinit(&exchange);
    cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  printf("Client: EDHOC Handshake Completed Successfully!\n");

  client_edhoc_flow_deinit(&flow);
  client_edhoc_exchange_deinit(&exchange);
  cleanup_resources(&client_resources);
  return EMULATION_SUCCESS;
}
