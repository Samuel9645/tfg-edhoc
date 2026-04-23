#include "coap/client/cli_exchange.h"

#include <edhoc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "coap/client/cli_log_error.h"
#include "coap/client/cli_utils.h"
#include "coap/common/com_coap_parse_edhoc_request.h"
#include "coap/common/com_coap_response.h"

// ReSharper disable once CppClassNeverUsed it is used, it is opaque
struct cli_coap_exchange {
  struct cli_coap_exchange_session_data session_data;
  bool have_response;
  struct com_writable_buffer incoming_response_buffer;

  struct {
    const uint8_t* bytes;
    size_t length;
  } internal_parsed_response;
};

static struct com_readonly_buffer get_readonly_buffer(
    const struct cli_coap_exchange* exchange) {
  return (struct com_readonly_buffer){
      .bytes = exchange->internal_parsed_response.bytes,
      .length = exchange->internal_parsed_response.length,
  };
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
static coap_response_t coap_client_coap_response_handler(
    coap_session_t* session, const coap_pdu_t* sent, const coap_pdu_t* received,
    const coap_mid_t id) {
  (void)sent;
  (void)id;
  struct cli_coap_exchange* exchange = coap_session_get_app_data(session);
  if (exchange == NULL) {
    coap_log_err("missing client exchange state in coap_response.handler\n");
    return COAP_RESPONSE_FAIL;
  }

  exchange->have_response = true;
  const coap_pdu_code_t response_code = coap_pdu_get_code(received);
  if (response_code == COAP_EMPTY_CODE) {
    coap_log_info("received empty response\n");
    return COAP_RESPONSE_OK;
  }

  const struct com_coap_parse_edhoc_request_result parse_result =
      com_coap_parse_edhoc_request(received, CONFIG_COAP_CONTENT_EDHOC,
                                   &exchange->incoming_response_buffer);

  exchange->internal_parsed_response.bytes = parse_result.parsed_request.bytes;
  exchange->internal_parsed_response.length =
      parse_result.parsed_request.length;
  if (response_code != COAP_RESPONSE_CODE_CHANGED) {
    cli_coap_log_received_edhoc_error_response(response_code,
                                               parse_result.parsed_request);
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
  return com_readonly_buffer_has_content(request_data.buffer);
}

struct cli_coap_exchange* cli_coap_init_exchange(
    const struct cli_coap_exchange_session_data* session_data,
    const struct com_writable_buffer response_buffer) {
  if (!cli_coap_exchange_session_data_is_valid(session_data)) {
    coap_log_err("invalid arguments to exchange_init\n");
    return NULL;
  }

  struct cli_coap_exchange* exchange =
      calloc(1, sizeof(struct cli_coap_exchange));
  if (exchange == NULL) {
    coap_log_err("failed calloc cli_coap_exchange\n");
    return NULL;
  }
  exchange->session_data = *session_data;
  // This is used to bypass the const limitation
  memcpy(&exchange->incoming_response_buffer, &response_buffer,
         sizeof(struct com_writable_buffer));
  // TODO: Note: It is the responsibility of the caller to free off (if
  // appropriate) any returned data.
  if (coap_session_set_app_data2(session_data->session, exchange, free) !=
      NULL) {
    coap_log_err("unexpected existing session app-data in client\n");
    free(exchange);
    return NULL;
  }
  coap_register_response_handler(session_data->context,
                                 coap_client_coap_response_handler);

  return exchange;
}

void reset(struct cli_coap_exchange* exchange) {
  if (exchange == NULL) {
    return;
  }

  exchange->have_response = false;
  exchange->internal_parsed_response.bytes = NULL;
  exchange->internal_parsed_response.length = 0;
  exchange->incoming_response_buffer.length = 0;
}

enum status_coap cli_coap_exchange_send(
    struct cli_coap_exchange* exchange,
    const struct cli_coap_exchange_request request_data) {
  reset(exchange);

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

static struct cli_coap_wait_and_get_result wait_and_get_ok(
    const struct com_readonly_buffer response_data) {
  return (struct cli_coap_wait_and_get_result){
      .status = STATUS_COAP_OK,
      .response = response_data,
  };
}

static struct cli_coap_wait_and_get_result wait_and_get_failure(void) {
  return (struct cli_coap_wait_and_get_result){.status = STATUS_COAP_ERR};
}

struct cli_coap_wait_and_get_result cli_coap_exchange_wait_and_get(
    const struct cli_coap_exchange* exchange) {
  if (exchange == NULL) {
    coap_log_err("invalid arguments to wait_and_get\n");
    return wait_and_get_failure();
  }
  const enum cli_coap_wait_status wait_status = cli_coap_wait_for_coap_response(
      exchange->session_data.context, exchange->session_data.session,
      &exchange->have_response);
  if (wait_status != CLI_COAP_WAIT_OK) {
    coap_log_err("error while waiting for CoAP response\n");
    return wait_and_get_failure();
  }
  const struct com_readonly_buffer response_buffer =
      get_readonly_buffer(exchange);
  if (!com_readonly_buffer_has_content(response_buffer)) {
    coap_log_err("no response received\n");
    return wait_and_get_failure();
  }
  return wait_and_get_ok(response_buffer);
}
