#include "coap/client/cli_exchange.h"

#include <edhoc_helpers.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "coap/client/cli_log_error.h"
#include "coap/client/cli_utils.h"
#include "coap/client/internal/cli_exchange_internal.h"
#include "coap/common/com_coap_get_data.h"
#include "coap/common/com_coap_parse_edhoc_request.h"
#include "coap/common/com_coap_pdu_helpers.h"

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
    coap_log_err("missing client exchange state in coap_response handler\n");
    return COAP_RESPONSE_FAIL;
  }

  exchange->have_response = true;
  const coap_pdu_code_t response_code = coap_pdu_get_code(received);
  if (response_code == COAP_EMPTY_CODE) {
    coap_log_info("received empty response\n");
    return COAP_RESPONSE_OK;
  }

  if (com_pdu_is_empty(received) && exchange->edhoc_exchange_is_complete) {
    coap_log_info("received Message 3 empty confirmation\n");
    exchange->internal_parsed_response.bytes = 0;
    exchange->internal_parsed_response.length = 0;
  } else {
    const struct com_coap_parse_edhoc_request_result parse_result =
        com_coap_parse_edhoc_request(received, CONFIG_COAP_CONTENT_EDHOC,
                                     exchange->incoming_response_buffer);
    if (parse_result.status != COM_COAP_PARSE_EDHOC_REQ_OK) {
      coap_log_err("failed to parse EDHOC response\n");
      return COAP_RESPONSE_FAIL;
    }
    exchange->internal_parsed_response.bytes =
        parse_result.parsed_request.bytes;
    exchange->internal_parsed_response.length =
        parse_result.parsed_request.length;
  }

  if (response_code != COAP_RESPONSE_CODE_CHANGED) {
    exchange->response_is_error = true;
    cli_coap_log_received_edhoc_error_response(response_code,
                                               get_readonly_buffer(exchange));
  } else {
    exchange->response_is_error = false;
  }
  return COAP_RESPONSE_OK;
}

void cli_coap_cleanup_exchange(struct cli_coap_exchange* exchange) {
  if (exchange == NULL) {
    return;
  }
  free(exchange);
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

void reset(struct cli_coap_exchange* exchange) {
  if (exchange == NULL) {
    return;
  }

  exchange->have_response = false;
  exchange->internal_parsed_response.bytes = NULL;
  exchange->internal_parsed_response.length = 0;
}

static enum status_coap default_send(
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
  if (com_coap_add_payload_to_pdu(request_pdu, request_data.buffer.bytes,
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
  exchange->send_data = default_send;
  // This is used to bypass the const limitation
  memcpy(&exchange->incoming_response_buffer, &response_buffer,
         sizeof(struct com_writable_buffer));
  void* previous_session_data =
      coap_session_set_app_data2(session_data->session, exchange, NULL);
  if (previous_session_data != NULL) {
    coap_log_warn(
        "Cleaning up previous exchange from failed negotiation attempt...\n");
    free(previous_session_data);
  }
  coap_register_response_handler(session_data->context,
                                 coap_client_coap_response_handler);
  return exchange;
}

enum status_coap cli_coap_exchange_send(
    struct cli_coap_exchange* exchange,
    const struct cli_coap_exchange_request request_data) {
  if (exchange->send_data == NULL) {
    coap_log_err(
        "INTERNAL ERROR: exchange send_data function pointer is NULL\n");
    return STATUS_COAP_ERR;
  }
  return exchange->send_data(exchange, request_data);
}

static struct cli_coap_wait_and_get_result wait_and_get_ok(
    const struct com_readonly_buffer response_data,
    const bool response_is_error) {
  return (struct cli_coap_wait_and_get_result){
      .status = STATUS_COAP_OK,
      .response = response_data,
      .response_is_error = response_is_error,
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
  if (!com_readonly_buffer_has_content(response_buffer) &&
      !exchange->edhoc_exchange_is_complete) {
    coap_log_err("no response received\n");
    return wait_and_get_failure();
  }
  return wait_and_get_ok(response_buffer, exchange->response_is_error);
}

enum status_coap cli_exchange_send_message_1(
    struct cli_coap_exchange* exchange,
    const struct com_readonly_buffer message_1) {
  if (exchange == NULL) {
    coap_log_err("Failed to send Message 1: exchange is NULL\n");
    return STATUS_COAP_ERR;
  }
  if (!com_readonly_buffer_has_content(message_1)) {
    coap_log_err("Failed to send Message 1: message buffer is empty\n");
    return STATUS_COAP_ERR;
  }
  if (message_1.length >= CONFIG_COAP_MAX_PDU_SIZE) {
    coap_log_err(
        "Failed to send Message 1: message size exceeds max PDU size\n");
    return STATUS_COAP_ERR;
  }

  uint8_t prepended_buffer[CONFIG_COAP_MAX_PDU_SIZE] = {0};
  struct edhoc_prepended_fields prepended_fields = {
      .buffer = prepended_buffer,
      .buffer_size = sizeof(prepended_buffer),
      .edhoc_message_ptr = prepended_buffer,
      .edhoc_message_size = sizeof(prepended_buffer),
  };

  if (edhoc_prepend_flow(&prepended_fields) != EDHOC_SUCCESS) {
    coap_log_err("Failed to send Message 1: failed to prepend flow\n");
    return STATUS_COAP_ERR;
  }
  if (prepended_fields.edhoc_message_size < message_1.length) {
    coap_log_err("Failed to send Message 1: buffer too small for message\n");
    return STATUS_COAP_ERR;
  }
  memcpy(prepended_fields.edhoc_message_ptr, message_1.bytes, message_1.length);
  prepended_fields.edhoc_message_size = message_1.length;
  if (edhoc_prepend_recalculate_size(&prepended_fields) != EDHOC_SUCCESS) {
    coap_log_err(
        "Failed to send Message 1: failed to recalculate prepended size\n");
    return STATUS_COAP_ERR;
  }
  const struct com_readonly_buffer prepended_message = {
      .bytes = prepended_fields.buffer,
      .length = prepended_fields.buffer_size,
  };
  const struct cli_coap_exchange_request request_data = {
      .buffer = prepended_message,
      .content_format = CONFIG_COAP_CONTENT_CID_EDHOC,
  };
  return cli_coap_exchange_send(exchange, request_data);
}

enum status_coap cli_exchange_send_message_3(
    struct cli_coap_exchange* exchange, const struct edhoc_context* context,
    const struct com_readonly_buffer message_3) {
  if (exchange == NULL) {
    coap_log_err("Failed to send Message 3: exchange is NULL\n");
    return STATUS_COAP_ERR;
  }
  if (context == NULL) {
    coap_log_err("Failed to send Message 3: context is NULL\n");
    return STATUS_COAP_ERR;
  }
  if (!com_readonly_buffer_has_content(message_3)) {
    coap_log_err("Failed to send Message 3: message buffer is empty\n");
    return STATUS_COAP_ERR;
  }
  if (message_3.length >= CONFIG_COAP_MAX_PDU_SIZE) {
    coap_log_err(
        "Failed to send Message 3: message size exceeds max PDU size\n");
    return STATUS_COAP_ERR;
  }

  uint8_t prepended_buffer[CONFIG_COAP_MAX_PDU_SIZE] = {0};
  struct edhoc_prepended_fields prepended_fields = {
      .buffer = prepended_buffer,
      .buffer_size = sizeof(prepended_buffer),
      .edhoc_message_ptr = prepended_buffer,
      .edhoc_message_size = sizeof(prepended_buffer),
  };
  if (edhoc_prepend_connection_id(
          &prepended_fields, &context->private_peer_cid) != EDHOC_SUCCESS) {
    coap_log_err("Failed to send Message 3: failed to prepend connection id\n");
    return STATUS_COAP_ERR;
  }
  if (prepended_fields.edhoc_message_size < message_3.length) {
    coap_log_err("Failed to send Message 3: buffer too small for message\n");
    return STATUS_COAP_ERR;
  }
  memcpy(prepended_fields.edhoc_message_ptr, message_3.bytes, message_3.length);
  prepended_fields.edhoc_message_size = message_3.length;
  if (edhoc_prepend_recalculate_size(&prepended_fields) != EDHOC_SUCCESS) {
    coap_log_err(
        "Failed to send Message 3: failed to recalculate prepended size\n");
    return STATUS_COAP_ERR;
  }
  const struct com_readonly_buffer prepended_message = {
      .bytes = prepended_fields.buffer,
      .length = prepended_fields.buffer_size,
  };
  const struct cli_coap_exchange_request request_data = {
      .buffer = prepended_message,
      .content_format = CONFIG_COAP_CONTENT_CID_EDHOC,
  };
  const enum status_coap send_status =
      cli_coap_exchange_send(exchange, request_data);
  if (send_status == STATUS_COAP_OK) {
    exchange->edhoc_exchange_is_complete = true;
  }
  return send_status;
}
