#include "core/client.h"

#include <coap3/coap.h>

#include "coap/client/cleanup.h"
#include "coap/client/exchange.h"
#include "coap/client/utils.h"
#include "coap/coap_config.h"
#include "edhoc/client/handshake.h"
#include "edhoc/config.h"

// TODO: SEND ERROR RESPONSES TO SERVER IN CASE OF FAILURE INSTEAD OF JUST
// FAILING SILENTLY

emulation_status_t tfg_run_client(void) {
  coap_startup();
  coap_set_log_level(COAP_LOG_WARN);

  coap_client_session_resources_t client_resources = {0};

  static const char CLIENT_COAP_URI[] =
      "coap://localhost:5683/.well-known/edhoc";
  coap_uri_t client_uri = {0};
  coap_address_t destination_address = {0};
  if (coap_client_parse_and_resolve_coap_uri(CLIENT_COAP_URI, &client_uri,
                                             &destination_address) !=
      CCOM_STATUS_SUCCESS) {
    coap_client_cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }
  if (coap_client_create_coap_session(
          &client_uri, &destination_address, NULL,
          &client_resources.session_resources.coap_context,
          &client_resources.session_resources.coap_session) !=
      CCOM_STATUS_SUCCESS) {
    coap_client_cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  coap_client_exchange_session_data_t exchange_session_data = {
      .context = client_resources.session_resources.coap_context,
      .session = client_resources.session_resources.coap_session,
      .uri = client_uri,
      .destination = destination_address,
  };

  if (coap_client_exchange_init(&exchange_session_data,
                                &client_resources.exchange) !=
      CCOM_STATUS_SUCCESS) {
    coap_client_cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  if (edhoc_client_handshake_init(&client_resources.handshake) !=
      EDHOC_CLIENT_HANDSHAKE_SUCCESS) {
    coap_client_cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  uint8_t request_payload[EDC_MESSAGE_BUFFER_LENGTH] = {0};
  uint8_t response_payload[MAX_PDU_SIZE] = {0};

  coap_client_exchange_request_data_t request_data = {
      .request_data =
          {
              .payload = request_payload,
              .payload_length = 0,
          },
      .content_format = APPLICATION_CID_EDHOC_CBOR_SEQ,
  };

  common_response_buffer_t response_data = {
      .payload = response_payload,
      .payload_capacity = MAX_PDU_SIZE,
      .payload_length = 0,
  };

  if (edhoc_client_handshake_compose_message_1(
          &client_resources.handshake, EDC_MESSAGE_BUFFER_LENGTH,
          request_payload, &request_data.request_data.payload_length) !=
      EDHOC_CLIENT_HANDSHAKE_SUCCESS) {
    coap_client_cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  if (coap_client_exchange_send(&client_resources.exchange, &request_data) !=
      CCOM_STATUS_SUCCESS) {
    coap_client_cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  if (coap_client_exchange_wait_and_get(
          &client_resources.exchange, &response_data) != CCOM_STATUS_SUCCESS ||
      edhoc_client_handshake_process_message_2(
          &client_resources.handshake, response_payload,
          response_data.payload_length) != EDHOC_CLIENT_HANDSHAKE_SUCCESS) {
    coap_client_cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  coap_client_exchange_reset(&client_resources.exchange);

  if (edhoc_client_handshake_compose_message_3(
          &client_resources.handshake, EDC_MESSAGE_BUFFER_LENGTH,
          request_payload, &request_data.request_data.payload_length) !=
      EDHOC_CLIENT_HANDSHAKE_SUCCESS) {
    coap_client_cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  if (coap_client_exchange_send(&client_resources.exchange, &request_data) !=
          CCOM_STATUS_SUCCESS ||
      coap_client_exchange_wait_and_get(
          &client_resources.exchange, &response_data) != CCOM_STATUS_SUCCESS ||
      edhoc_client_handshake_process_message_4(
          &client_resources.handshake, response_payload,
          response_data.payload_length) != EDHOC_CLIENT_HANDSHAKE_SUCCESS) {
    coap_client_cleanup_resources(&client_resources);
    return EMULATION_FAILURE;
  }

  coap_client_cleanup_resources(&client_resources);
  return EMULATION_SUCCESS;
}
