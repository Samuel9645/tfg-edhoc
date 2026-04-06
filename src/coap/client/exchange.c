#include "coap/client/exchange.h"

#include <edhoc.h>
#include <stdbool.h>
#include <string.h>

#include "coap/client/cli_utils.h"
#include "coap/client/log_error_response.h"
#include "coap/common/cp_get_data.h"
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

  struct cp_cli_exchange* exchange = coap_session_get_app_data(session);
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

  const struct cp_com_get_data_result get_data_result =
      cp_com_get_data(received);
  if (get_data_result.status != CP_COM_GET_DATA_OK) {
    coap_log_err("cannot get response pdu data\n");
    return COAP_RESPONSE_OK;
  }
  const struct com_readonly_buffer data = get_data_result.output;
  memcpy(exchange->incoming_message, data.bytes, data.length);
  exchange->incoming_message_length = data.length;
  if (response_code != COAP_RESPONSE_CODE_CHANGED) {
    cp_cli_log_received_edhoc_error_response(response_code, data.bytes,
                                             data.length);
  }
  return COAP_RESPONSE_OK;
}

bool cp_cli_exchange_session_data_is_valid(
    const struct cp_cli_exchange_session_data* session_data) {
  return session_data != NULL && session_data->context != NULL &&
         session_data->session != NULL;
}

bool cp_cli_exchange_request_data_is_valid(
    const struct cp_cli_exchange_request request_data) {
  return com_readonly_buffer_is_valid(request_data.buffer);
}

enum cp_status cp_cli_init_exchange(
    const struct cp_cli_exchange_session_data* session_data,
    struct cp_cli_exchange* exchange) {
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

enum cp_status cp_cli_exchange_send(
    const struct cp_cli_exchange* exchange,
    const struct cp_cli_exchange_request request_data) {
  if (exchange == NULL ||
      !cp_cli_exchange_request_data_is_valid(request_data)) {
    coap_log_err("invalid arguments to exchange_send\n");
    return CP_STATUS_ERROR;
  }
  const struct cp_cli_session_config config = {
      .uri = &exchange->session_data.uri,
      .address = &exchange->session_data.destination,
  };
  const struct cp_cli_prepare_pdu_result prepare_pdu_result =
      cp_cli_prepare_post_request(config, exchange->session_data.session,
                                  request_data.content_format);
  if (prepare_pdu_result.status != CP_CLI_PREPARE_PDU_OK) {
    coap_log_err("failed to prepare CoAP request\n");
    return CP_STATUS_ERROR;
  }

  coap_pdu_t* request_pdu = prepare_pdu_result.pdu;
  if (cp_com_add_response_payload(request_pdu, request_data.buffer.bytes,
                                  request_data.buffer.length) ==
      CP_STATUS_ERROR) {
    coap_log_err("cannot add payload to request PDU\n");
    coap_delete_pdu(request_pdu);
    return CP_STATUS_ERROR;
  }

  coap_show_pdu(COAP_LOG_WARN, request_pdu);
  return cp_cli_send_coap_request(exchange->session_data.session, request_pdu);
}

enum cp_status cp_cli_exchange_wait_and_get(
    struct cp_cli_exchange* exchange,
    struct com_writable_buffer* response_data) {
  if (exchange == NULL || !com_writable_buffer_is_writable(response_data)) {
    coap_log_err("invalid arguments to wait_and_get\n");
    return CP_STATUS_ERROR;
  }
  const enum cp_cli_wait_status wait_status = cp_cli_wait_for_coap_response(
      exchange->session_data.context, exchange->session_data.session,
      &exchange->have_response);
  if (wait_status != CP_CLI_WAIT_OK) {
    coap_log_err("error while waiting for CoAP response\n");
    return CP_STATUS_ERROR;
  }

  const size_t message_length = exchange->incoming_message_length;
  const size_t capacity = response_data->capacity;
  if (!exchange->have_response ||
      !(message_length > 0 && message_length <= capacity)) {
    coap_log_err("invalid response data\n");
    return CP_STATUS_ERROR;
  }

  const bool is_error_response =
      !cp_com_coap_response_indicates_success(exchange->last_response_code);

  memcpy(response_data->bytes, exchange->incoming_message,
         exchange->incoming_message_length);
  response_data->length = exchange->incoming_message_length;
  exchange->have_response = false;
  exchange->incoming_message_length = 0;
  exchange->last_response_code = COAP_EMPTY_CODE;

  return is_error_response ? CP_STATUS_ERROR : CP_STATUS_SUCCESS;
}

void cp_cli_exchange_reset(struct cp_cli_exchange* exchange) {
  if (exchange == NULL) {
    return;
  }

  exchange->have_response = false;
  exchange->incoming_message_length = 0;
  exchange->last_response_code = COAP_EMPTY_CODE;
}
