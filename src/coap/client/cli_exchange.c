#include "coap/client/cli_exchange.h"

#include <edhoc.h>
#include <stdbool.h>
#include <string.h>

#include "coap/client/cli_log_error.h"
#include "coap/client/cli_utils.h"
#include "coap/common/coap_get_data.h"
#include "coap/common/coap_helpers.h"
#include "coap/common/coap_response.h"

static bool client_coap_response_has_edhoc_content_format(
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
  return content_format == CONFIG_COAP_CONTENT_EDHOC;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
static coap_response_t coap_client_coap_response_handler(
    coap_session_t* session, const coap_pdu_t* sent, const coap_pdu_t* received,
    const coap_mid_t id) {
  (void)sent;
  (void)id;

  struct cli_coap_exchange* exchange = coap_session_get_app_data(session);
  if (!exchange) {
    coap_log_err("missing client exchange state in coap_response.handler\n");
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

  if (!client_coap_response_has_edhoc_content_format(received)) {
    coap_log_err("missing or invalid EDHOC content format in response\n");
    return COAP_RESPONSE_OK;
  }

  const struct com_coap_get_data_result get_data_result =
      com_coap_get_data(received);
  if (get_data_result.status != COM_COAP_GET_DATA_OK) {
    coap_log_err("cannot get response pdu data\n");
    return COAP_RESPONSE_OK;
  }
  const struct com_readonly_buffer data = get_data_result.output;
  memcpy(exchange->incoming_message, data.bytes, data.length);
  exchange->incoming_message_length = data.length;
  if (response_code != COAP_RESPONSE_CODE_CHANGED) {
    cli_coap_log_received_edhoc_error_response(response_code, data.bytes,
                                               data.length);
  }
  return COAP_RESPONSE_OK;
}

bool cli_coap_exchange_session_data_is_valid(
    const struct cli_coap_exchange_session_data* session_data) {
  return session_data != NULL && session_data->context != NULL &&
         session_data->session != NULL;
}

bool cli_coap_exchange_request_data_is_valid(
    const struct cli_coap_exchange_request request_data) {
  return com_readonly_buffer_is_valid(request_data.buffer);
}

enum status_coap cli_coap_init_exchange(
    const struct cli_coap_exchange_session_data* session_data,
    struct cli_coap_exchange* exchange) {
  if (!exchange || !cli_coap_exchange_session_data_is_valid(session_data)) {
    coap_log_err("invalid arguments to exchange_init\n");
    return STATUS_COAP_ERR;
  }

  memset(exchange, 0, sizeof(*exchange));

  exchange->session_data.context = session_data->context;
  exchange->session_data.session = session_data->session;
  exchange->session_data.uri = session_data->uri;
  exchange->session_data.destination = session_data->destination;

  if (coap_session_set_app_data2(session_data->session, exchange, NULL) !=
      NULL) {
    coap_log_err("unexpected existing session app-data in client\n");
    return STATUS_COAP_ERR;
  }

  coap_register_response_handler(session_data->context,
                                 coap_client_coap_response_handler);

  return STATUS_COAP_OK;
}

enum status_coap cli_coap_exchange_send(
    const struct cli_coap_exchange* exchange,
    const struct cli_coap_exchange_request request_data) {
  if (exchange == NULL ||
      !cli_coap_exchange_request_data_is_valid(request_data)) {
    coap_log_err("invalid arguments to exchange_send\n");
    return STATUS_COAP_ERR;
  }
  const struct cli_coap_session_config config = {
      .uri = &exchange->session_data.uri,
      .address = &exchange->session_data.destination,
  };
  const struct cli_coap_prepare_pdu_result prepare_pdu_result =
      cli_coap_prepare_post_request(config, exchange->session_data.session,
                                    request_data.content_format);
  if (prepare_pdu_result.status != CLI_COAP_PREPARE_PDU_OK) {
    coap_log_err("failed to prepare CoAP request\n");
    return STATUS_COAP_ERR;
  }

  coap_pdu_t* request_pdu = prepare_pdu_result.pdu;
  if (com_coap_add_response_payload(request_pdu, request_data.buffer.bytes,
                                    request_data.buffer.length) ==
      STATUS_COAP_ERR) {
    coap_log_err("cannot add payload to request PDU\n");
    coap_delete_pdu(request_pdu);
    return STATUS_COAP_ERR;
  }

  coap_show_pdu(COAP_LOG_WARN, request_pdu);
  return cli_coap_send_coap_request(exchange->session_data.session,
                                    request_pdu);
}

enum status_coap cli_coap_exchange_wait_and_get(
    struct cli_coap_exchange* exchange,
    struct com_writable_buffer* response_data) {
  if (exchange == NULL || !com_writable_buffer_is_writable(response_data)) {
    coap_log_err("invalid arguments to wait_and_get\n");
    return STATUS_COAP_ERR;
  }
  const enum cli_coap_wait_status wait_status = cli_coap_wait_for_coap_response(
      exchange->session_data.context, exchange->session_data.session,
      &exchange->have_response);
  if (wait_status != CLI_COAP_WAIT_OK) {
    coap_log_err("error while waiting for CoAP response\n");
    return STATUS_COAP_ERR;
  }

  const size_t message_length = exchange->incoming_message_length;
  const size_t capacity = response_data->capacity;
  if (!exchange->have_response ||
      !(message_length > 0 && message_length <= capacity)) {
    coap_log_err("invalid response data\n");
    return STATUS_COAP_ERR;
  }

  const bool is_error_response =
      !com_coap_coap_response_indicates_success(exchange->last_response_code);

  memcpy(response_data->bytes, exchange->incoming_message,
         exchange->incoming_message_length);
  response_data->length = exchange->incoming_message_length;
  exchange->have_response = false;
  exchange->incoming_message_length = 0;
  exchange->last_response_code = COAP_EMPTY_CODE;

  return is_error_response ? STATUS_COAP_ERR : STATUS_COAP_OK;
}

void cli_coap_exchange_reset(struct cli_coap_exchange* exchange) {
  if (exchange == NULL) {
    return;
  }

  exchange->have_response = false;
  exchange->incoming_message_length = 0;
  exchange->last_response_code = COAP_EMPTY_CODE;
}
