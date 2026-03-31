#include "coap/client/exchange.h"

#include <edhoc.h>
#include <stdbool.h>
#include <string.h>

#include "coap/client/log_error_response.h"
#include "coap/client/utils.h"
#include "coap/common/helpers.h"
#include "coap/common/response.h"

static bool client_response_has_edhoc_content_format(
    const coap_pdu_t* response) {
  coap_opt_iterator_t option_iterator = {0};
  coap_opt_t* content_format_option =
      coap_check_option(response, COAP_OPTION_CONTENT_FORMAT, &option_iterator);
  if (!content_format_option) {
    return false;
  }

  uint16_t content_format =
      coap_decode_var_bytes(coap_opt_value(content_format_option),
                            coap_opt_length(content_format_option));
  return content_format == APPLICATION_EDHOC_CBOR_SEQ;
}

static coap_response_t coap_client_response_handler(coap_session_t* session,
                                                    const coap_pdu_t* sent,
                                                    const coap_pdu_t* received,
                                                    const coap_mid_t id) {
  (void)sent;
  (void)id;

  coap_client_exchange_t* exchange =
      (coap_client_exchange_t*)coap_session_get_app_data(session);
  if (!exchange) {
    coap_log_err("missing client exchange state in response handler\n");
    return COAP_RESPONSE_FAIL;
  }

  exchange->have_response = true;
  exchange->incoming_message_len = 0;

  coap_pdu_code_t response_code = coap_pdu_get_code(received);
  exchange->last_response_code = response_code;
  if (response_code == COAP_EMPTY_CODE) {
    coap_log_info("received empty response\n");
    return COAP_RESPONSE_OK;
  }

  if (!client_response_has_edhoc_content_format(received)) {
    coap_log_err("missing or invalid EDHOC content format in response\n");
    return COAP_RESPONSE_OK;
  }

  size_t payload_len = 0;
  const uint8_t* payload = NULL;
  if (!coap_get_data(received, &payload_len, &payload) || payload_len == 0) {
    coap_log_err("cannot get response pdu data\n");
    return COAP_RESPONSE_OK;
  }

  if (payload_len > sizeof(exchange->incoming_message)) {
    coap_log_err("response payload too large\n");
    return COAP_RESPONSE_OK;
  }

  memcpy(exchange->incoming_message, payload, payload_len);
  exchange->incoming_message_len = payload_len;

  if (response_code != COAP_RESPONSE_CODE_CHANGED) {
    coap_client_log_received_edhoc_error_response(response_code, payload,
                                                  payload_len);
    return COAP_RESPONSE_OK;
  }

  return COAP_RESPONSE_OK;
}

coap_status_result_t coap_client_exchange_init(
    const coap_client_exchange_session_data_t* session_data,
    coap_client_exchange_t* exchange) {
  if (!exchange || !session_data || !session_data->session_data.context ||
      !session_data->session_data.session || !session_data->endpoint_data.uri ||
      !session_data->endpoint_data.destination) {
    coap_log_err("invalid arguments to exchange_init\n");
    return CCOM_ERROR;
  }

  memset(exchange, 0, sizeof(*exchange));

  exchange->context = session_data->session_data.context;
  exchange->session = session_data->session_data.session;
  exchange->uri = *session_data->endpoint_data.uri;
  exchange->destination = *session_data->endpoint_data.destination;

  if (coap_session_set_app_data2(session_data->session_data.session, exchange,
                                 NULL) != NULL) {
    coap_log_err("unexpected existing session app-data in client\n");
    return CCOM_ERROR;
  }

  coap_register_response_handler(session_data->session_data.context,
                                 coap_client_response_handler);

  return CCOM_STATUS_SUCCESS;
}

coap_status_result_t coap_client_exchange_send(
    coap_client_exchange_t* exchange,
    const coap_client_exchange_request_data_t* request_data) {
  if (!exchange || !request_data ||
      !coap_client_exchange_request_data_is_valid(request_data)) {
    coap_log_err("invalid arguments to exchange_send\n");
    return CCOM_ERROR;
  }

  coap_pdu_t* request_pdu = coap_client_prepare_post_request(
      &exchange->uri, &exchange->destination, exchange->session,
      request_data->content_format);
  if (!request_pdu) {
    coap_log_err("failed to prepare CoAP request\n");
    return CCOM_ERROR;
  }

  if (coap_common_add_response_payload(
          request_pdu, request_data->request_data.payload,
          request_data->request_data.payload_len) == CCOM_ERROR) {
    coap_log_err("cannot add payload to request PDU\n");
    coap_delete_pdu(request_pdu);
    return CCOM_ERROR;
  }

  coap_show_pdu(COAP_LOG_WARN, request_pdu);
  return coap_client_send_coap_request(exchange->session, request_pdu);
}

coap_status_result_t coap_client_exchange_wait_and_get(
    coap_client_exchange_t* exchange, common_response_buffer_t* response_data) {
  if (!exchange || !common_response_buffer_is_writable(response_data)) {
    coap_log_err("invalid arguments to wait_and_get\n");
    return CCOM_ERROR;
  }

  if (coap_client_wait_for_coap_response(exchange->context, exchange->session,
                                         &exchange->have_response) !=
      CCOM_STATUS_SUCCESS) {
    coap_log_err("error while waiting for CoAP response\n");
    return CCOM_ERROR;
  }

  if (!exchange->have_response || exchange->incoming_message_len == 0 ||
      exchange->incoming_message_len > response_data->payload_capacity) {
    coap_log_err("invalid response data\n");
    return CCOM_ERROR;
  }

  bool is_error_response =
      exchange->last_response_code != COAP_RESPONSE_CODE_CHANGED;

  memcpy(response_data->payload, exchange->incoming_message,
         exchange->incoming_message_len);
  response_data->payload_len = exchange->incoming_message_len;
  exchange->have_response = false;
  exchange->incoming_message_len = 0;
  exchange->last_response_code = COAP_EMPTY_CODE;

  return is_error_response ? CCOM_ERROR : CCOM_STATUS_SUCCESS;
}

void coap_client_exchange_reset(coap_client_exchange_t* exchange) {
  if (!exchange) {
    return;
  }

  exchange->have_response = false;
  exchange->incoming_message_len = 0;
  exchange->last_response_code = COAP_EMPTY_CODE;
}
