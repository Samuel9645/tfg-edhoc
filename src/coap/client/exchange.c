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

  const uint16_t content_format =
      coap_decode_var_bytes(coap_opt_value(content_format_option),
                            coap_opt_length(content_format_option));
  return content_format == CP_CFG_CONTENT_EDHOC;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
static coap_response_t coap_client_response_handler(coap_session_t* session,
                                                    const coap_pdu_t* sent,
                                                    const coap_pdu_t* received,
                                                    const coap_mid_t id) {
  (void)sent;
  (void)id;

  cp_cli_exchange_t* exchange = coap_session_get_app_data(session);
  if (!exchange) {
    coap_log_err("missing client exchange state in response handler\n");
    return COAP_RESPONSE_FAIL;
  }

  exchange->have_response = true;
  exchange->incoming_message_length = 0;

  const coap_pdu_code_t response_code = coap_pdu_get_code(received);
  exchange->last_response_code = response_code;
  if (response_code == COAP_EMPTY_CODE) {
    coap_log_info("received empty response\n");
    return COAP_RESPONSE_OK;
  }

  if (!client_response_has_edhoc_content_format(received)) {
    coap_log_err("missing or invalid EDHOC content format in response\n");
    return COAP_RESPONSE_OK;
  }

  size_t payload_length = 0;
  const uint8_t* payload = NULL;
  if (!coap_get_data(received, &payload_length, &payload) ||
      payload_length == 0) {
    coap_log_err("cannot get response pdu data\n");
    return COAP_RESPONSE_OK;
  }

  if (payload_length > sizeof(exchange->incoming_message)) {
    coap_log_err("response payload too large\n");
    return COAP_RESPONSE_OK;
  }

  memcpy(exchange->incoming_message, payload, payload_length);
  exchange->incoming_message_length = payload_length;

  if (response_code != COAP_RESPONSE_CODE_CHANGED) {
    cp_cli_log_received_edhoc_error_response(response_code, payload,
                                             payload_length);
    return COAP_RESPONSE_OK;
  }

  return COAP_RESPONSE_OK;
}

bool cp_cli_exchange_session_data_is_valid(
    const cp_cli_exchange_session_data_t* session_data) {
  return session_data != NULL && session_data->context != NULL &&
         session_data->session != NULL;
}

bool cp_cli_exchange_request_data_is_valid(
    const cp_cli_exchange_request_data_t* request_data) {
  return request_data != NULL &&
         com_request_payload_is_valid(&request_data->request_data);
}

bool cp_cli_exchange_response_size_fits(const size_t message_length,
                                        const size_t capacity) {
  return message_length > 0 && message_length <= capacity;
}

cp_status_t cp_cli_exchange_init(
    const cp_cli_exchange_session_data_t* session_data,
    cp_cli_exchange_t* exchange) {
  if (!exchange || !cp_cli_exchange_session_data_is_valid(session_data)) {
    coap_log_err("invalid arguments to exchange_init\n");
    return CP_STATUS_ERROR;
  }

  memset(exchange, 0, sizeof(*exchange));

  exchange->session_data.context = session_data->context;
  exchange->session_data.session = session_data->session;
  exchange->session_data.uri = session_data->uri;
  exchange->session_data.destination = session_data->destination;

  if (coap_session_set_app_data2(session_data->session, exchange, NULL) !=
      NULL) {
    coap_log_err("unexpected existing session app-data in client\n");
    return CP_STATUS_ERROR;
  }

  coap_register_response_handler(session_data->context,
                                 coap_client_response_handler);

  return CP_STATUS_SUCCESS;
}

cp_status_t cp_cli_exchange_send(
    cp_cli_exchange_t* exchange,
    const cp_cli_exchange_request_data_t* request_data) {
  if (!exchange || !request_data ||
      !cp_cli_exchange_request_data_is_valid(request_data)) {
    coap_log_err("invalid arguments to exchange_send\n");
    return CP_STATUS_ERROR;
  }

  coap_pdu_t* request_pdu = cp_cli_prepare_post_request(
      &exchange->session_data.uri, &exchange->session_data.destination,
      exchange->session_data.session, request_data->content_format);
  if (!request_pdu) {
    coap_log_err("failed to prepare CoAP request\n");
    return CP_STATUS_ERROR;
  }

  if (cp_com_add_response_payload(
          request_pdu, request_data->request_data.buffer,
          request_data->request_data.length) == CP_STATUS_ERROR) {
    coap_log_err("cannot add payload to request PDU\n");
    coap_delete_pdu(request_pdu);
    return CP_STATUS_ERROR;
  }

  coap_show_pdu(COAP_LOG_WARN, request_pdu);
  return cp_cli_send_coap_request(exchange->session_data.session, request_pdu);
}

cp_status_t cp_cli_exchange_wait_and_get(cp_cli_exchange_t* exchange, com_response_buffer_t* response_data) {
  if (!exchange || !com_response_buffer_is_writable(response_data)) {
    coap_log_err("invalid arguments to wait_and_get\n");
    return CP_STATUS_ERROR;
  }

  if (cp_cli_wait_for_coap_response(
          exchange->session_data.context, exchange->session_data.session,
          &exchange->have_response) != CP_STATUS_SUCCESS) {
    coap_log_err("error while waiting for CoAP response\n");
    return CP_STATUS_ERROR;
  }

  if (!exchange->have_response ||
      !cp_cli_exchange_response_size_fits(exchange->incoming_message_length,
                                          response_data->capacity)) {
    coap_log_err("invalid response data\n");
    return CP_STATUS_ERROR;
  }

  const bool is_error_response =
      !cp_com_coap_response_indicates_success(exchange->last_response_code);

  memcpy(response_data->buffer, exchange->incoming_message,
         exchange->incoming_message_length);
  response_data->length = exchange->incoming_message_length;
  exchange->have_response = false;
  exchange->incoming_message_length = 0;
  exchange->last_response_code = COAP_EMPTY_CODE;

  return is_error_response ? CP_STATUS_ERROR : CP_STATUS_SUCCESS;
}

void cp_cli_exchange_reset(cp_cli_exchange_t* exchange) {
  if (!exchange) {
    return;
  }

  exchange->have_response = false;
  exchange->incoming_message_length = 0;
  exchange->last_response_code = COAP_EMPTY_CODE;
}
