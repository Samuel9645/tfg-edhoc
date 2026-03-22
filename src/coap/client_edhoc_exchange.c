#include "coap/client_edhoc_exchange.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "coap/client_utils.h"
#include "coap/common/helpers.h"

typedef struct client_edhoc_exchange_t {
  coap_context_t* context;
  coap_session_t* session;
  coap_uri_t uri;
  coap_address_t destination;
  bool have_response;
  uint8_t incoming_message[MAX_PDU_SIZE];
  size_t incoming_message_len;
} client_edhoc_exchange_t;

static coap_response_t client_edhoc_response_handler(coap_session_t* session,
                                                     const coap_pdu_t* sent,
                                                     const coap_pdu_t* received,
                                                     const coap_mid_t id) {
  (void)sent;
  (void)id;

  client_edhoc_exchange_t* exchange =
      (client_edhoc_exchange_t*)coap_session_get_app_data(session);
  if (!exchange) {
    coap_log_err("missing client exchange state in response handler\n");
    return COAP_RESPONSE_FAIL;
  }

  coap_pdu_code_t response_code = coap_pdu_get_code(received);
  if (response_code == COAP_EMPTY_CODE) {
    coap_log_info("received empty response\n");
    return COAP_RESPONSE_OK;
  }

  if (response_code != COAP_RESPONSE_CODE_CHANGED) {
    coap_log_err("received error response code: %d.%02d\n", response_code >> 5,
                 response_code & 0x1F);
    return COAP_RESPONSE_FAIL;
  }

  size_t payload_len = 0;
  const uint8_t* payload = NULL;
  if (!coap_get_data(received, &payload_len, &payload)) {
    coap_log_err("cannot get response pdu data\n");
    return COAP_RESPONSE_FAIL;
  }

  if (payload_len > sizeof(exchange->incoming_message)) {
    coap_log_err("response payload too large\n");
    return COAP_RESPONSE_FAIL;
  }

  memcpy(exchange->incoming_message, payload, payload_len);
  exchange->incoming_message_len = payload_len;
  exchange->have_response = true;
  return COAP_RESPONSE_OK;
}

coap_status_result_t client_edhoc_exchange_init(
    coap_context_t* context, coap_session_t* session, const coap_uri_t* uri,
    const coap_address_t* destination, client_edhoc_exchange_t** exchange_out) {
  if (!exchange_out || !context || !session || !uri || !destination) {
    return COAP_STATUS_ERROR;
  }

  client_edhoc_exchange_t* exchange =
      calloc(1, sizeof(client_edhoc_exchange_t));
  if (!exchange) {
    return COAP_STATUS_ERROR;
  }

  exchange->context = context;
  exchange->session = session;
  exchange->uri = *uri;
  exchange->destination = *destination;

  if (coap_session_set_app_data2(session, exchange, NULL) != NULL) {
    coap_log_err("unexpected existing session app-data in client\n");
    free(exchange);
    return COAP_STATUS_ERROR;
  }

  coap_register_response_handler(context, client_edhoc_response_handler);

  *exchange_out = exchange;
  return COAP_STATUS_SUCCESS;
}

coap_status_result_t client_edhoc_exchange_send(
    client_edhoc_exchange_t* exchange, const uint8_t* payload,
    size_t payload_len, content_format_edhoc_values_t content_format) {
  if (!exchange || !payload || payload_len == 0) {
    return COAP_STATUS_ERROR;
  }

  coap_optlist_t* optlist = create_coap_edhoc_optlist(content_format);
  if (!optlist) {
    return COAP_STATUS_ERROR;
  }

  coap_pdu_t* request_pdu = prepare_coap_post_request(
      &exchange->uri, &exchange->destination, exchange->session, optlist);
  if (!request_pdu) {
    return COAP_STATUS_ERROR;
  }

  if (coap_add_data(request_pdu, payload_len, payload) == LIBCOAP_ERROR) {
    coap_log_err("cannot add payload to request PDU\n");
    coap_delete_pdu(request_pdu);
    return COAP_STATUS_ERROR;
  }

  coap_show_pdu(COAP_LOG_WARN, request_pdu);
  return send_coap_request(exchange->session, request_pdu);
}

coap_status_result_t client_edhoc_exchange_wait_and_get(
    client_edhoc_exchange_t* exchange, size_t response_capacity,
    uint8_t* response_buffer, size_t* response_len) {
  if (!exchange || !response_buffer || !response_len ||
      response_capacity == 0) {
    return COAP_STATUS_ERROR;
  }

  if (wait_for_coap_response(exchange->context, exchange->session,
                             &exchange->have_response) != COAP_STATUS_SUCCESS) {
    return COAP_STATUS_ERROR;
  }

  if (!exchange->have_response || exchange->incoming_message_len == 0 ||
      exchange->incoming_message_len > response_capacity) {
    return COAP_STATUS_ERROR;
  }

  memcpy(response_buffer, exchange->incoming_message,
         exchange->incoming_message_len);
  *response_len = exchange->incoming_message_len;
  exchange->have_response = false;
  exchange->incoming_message_len = 0;

  return COAP_STATUS_SUCCESS;
}

void client_edhoc_exchange_reset(client_edhoc_exchange_t* exchange) {
  if (!exchange) {
    return;
  }

  exchange->have_response = false;
  exchange->incoming_message_len = 0;
}

void client_edhoc_exchange_deinit(client_edhoc_exchange_t** exchange_ptr) {
  if (!exchange_ptr || !*exchange_ptr) {
    return;
  }

  if ((*exchange_ptr)->session) {
    coap_session_set_app_data2((*exchange_ptr)->session, NULL, NULL);
  }

  free(*exchange_ptr);
  *exchange_ptr = NULL;
}
