/**
 * @file
 *
 * @brief Client-side utilities for CoAP.
 *
 * @note Portions of this code are adapted from the libcoap-minimal client.cc
 * example.
 * @see [libcoap-minimal
 * repository](https://github.com/obgm/libcoap-minimal.git)
 *
 * Copyright (C) 2018-2024 Olaf Bergmann <bergmann@tzi.org>
 */

#include "coap/client/cli_utils.h"

#include <coap3/coap.h>
#include <stdbool.h>
#include <string.h>

#include "coap/coap_config.h"
#include "coap/common/com_coap_helpers.h"
#include "coap/common/com_coap_status.h"

// TODO: PARAMETER VALIDATION AND ERROR HANDLING

static struct cli_coap_parse_and_resolve_result parse_and_resolve_failure(
    const enum cli_coap_parse_and_resolve_status status) {
  return (struct cli_coap_parse_and_resolve_result){.status = status};
}

static struct cli_coap_parse_and_resolve_result parse_and_resolve_success(
    const coap_uri_t uri, const coap_address_t address) {
  return (struct cli_coap_parse_and_resolve_result){
      .status = CLI_COAP_PARSE_AND_RESOLVE_OK, .uri = uri, .address = address};
}

struct cli_coap_parse_and_resolve_result cli_coap_parse_and_resolve_coap_uri(
    const char* uri_string) {
  coap_address_t destination_address;
  coap_uri_t parsed_uri;
  if (coap_split_uri((const uint8_t*)uri_string, strlen(uri_string),
                     &parsed_uri) != 0) {
    coap_log_err("cannot parse uri %s\n", uri_string);
    return parse_and_resolve_failure(
        CLI_COAP_PARSE_AND_RESOLVE_ERR_INVALID_URI);
  }

  const uint32_t masked_protocol = 1 << parsed_uri.scheme;
  const enum status_coap resolve_status =
      com_coap_resolve_address(&parsed_uri.host, parsed_uri.port,
                               (int)masked_protocol, &destination_address);
  if (resolve_status != STATUS_COAP_OK) {
    coap_log_err("cannot resolve address %*.*s\n", (int)parsed_uri.host.length,
                 (int)parsed_uri.host.length, (const char*)parsed_uri.host.s);
    return parse_and_resolve_failure(
        CLI_COAP_PARSE_AND_RESOLVE_ERR_ADDRESS_RESOLUTION);
  }
  return parse_and_resolve_success(parsed_uri, destination_address);
}

static struct cli_coap_create_session_result create_session_result_failure(
    void) {
  return (struct cli_coap_create_session_result){
      .status = CLI_COAP_CREATE_SESSION_ERR,
      .session = NULL,
  };
}

static struct cli_coap_create_session_result create_session_success(
    coap_session_t* session) {
  return (struct cli_coap_create_session_result){
      .status = CLI_COAP_CREATE_SESSION_OK,
      .session = session,
  };
}

struct cli_coap_create_session_result cli_coap_create_session(
    coap_context_t* context, const struct cli_coap_session_config config) {
  const coap_proto_t protocol = config.uri->scheme == COAP_URI_SCHEME_COAP_TCP
                                    ? COAP_PROTO_TCP
                                    : COAP_PROTO_UDP;
  const coap_address_t* suitable_local_interface_option = NULL;
  coap_session_t* session = coap_new_client_session(
      context, suitable_local_interface_option, config.address, protocol);
  if (session == NULL) {
    coap_log_err("failed to create CoAP session\n");
    return create_session_result_failure();
  }
  return create_session_success(session);
}

static coap_pdu_t* create_request_pdu(coap_session_t* coap_session,
                                      const coap_pdu_code_t message_code) {
  coap_pdu_t* pdu = coap_pdu_init(COAP_MESSAGE_CON, message_code,
                                  coap_new_message_id(coap_session),
                                  coap_session_max_pdu_size(coap_session));
  if (pdu == NULL) {
    return NULL;
  }

  enum { REQUIRED_TOKEN_SIZE = 8 };

  uint8_t token[REQUIRED_TOKEN_SIZE];
  size_t token_len;
  coap_session_new_token(coap_session, &token_len, token);
  if (!coap_add_token(pdu, token_len, token)) {
    coap_log_err("Failed to add token to PDU\n");
    coap_delete_pdu(pdu);
    return NULL;
  }
  return pdu;
}

static int add_uri_into_optlist(const coap_uri_t* client_uri,
                                const coap_address_t* destination_address,
                                coap_optlist_t** optlist) {
  enum { CCU_ADD_PORT_OPTION = 1 };

  return coap_uri_into_optlist(client_uri, destination_address, optlist,
                               CCU_ADD_PORT_OPTION);
}

static int add_uri_into_pdu(const coap_uri_t* client_uri,
                            const coap_address_t* destination_address,
                            coap_optlist_t** options_list,
                            coap_pdu_t* request_pdu) {
  enum { CCU_COAP_ERROR_INDICATOR = 0 };

  int result =
      add_uri_into_optlist(client_uri, destination_address, options_list);
  if (result == CCU_COAP_ERROR_INDICATOR) {
    coap_log_err("cannot add URI into options list\n");
    return result;
  }
  result = coap_add_optlist_pdu(request_pdu, options_list);
  if (result == CCU_COAP_ERROR_INDICATOR) {
    coap_log_err("cannot add options to PDU\n");
    return result;
  }
  return result;
}

static struct cli_coap_prepare_pdu_result prepare_pdu_result_failure(
    const enum cli_coap_prepare_pdu_status status) {
  return (struct cli_coap_prepare_pdu_result){.status = status, .pdu = NULL};
}

static struct cli_coap_prepare_pdu_result prepare_pdu_ok(coap_pdu_t* pdu) {
  return (struct cli_coap_prepare_pdu_result){.status = CLI_COAP_PREPARE_PDU_OK,
                                              .pdu = pdu};
}

struct cli_coap_prepare_pdu_result cli_coap_prepare_post_request(
    const struct cli_coap_session_config config, coap_session_t* coap_session,
    const enum config_coap_content_format_edhoc_values content_format) {
  coap_pdu_t* request_pdu =
      create_request_pdu(coap_session, COAP_REQUEST_CODE_POST);
  if (request_pdu == NULL) {
    coap_log_err("cannot create PDU\n");
    return prepare_pdu_result_failure(CLI_COAP_PREPARE_PDU_ERR_CREATE_PDU);
  }
  coap_optlist_t* optlist = com_coap_create_coap_edhoc_optlist(content_format);
  if (optlist == NULL) {
    coap_log_err("cannot create options list\n");
    coap_delete_pdu(request_pdu);
    return prepare_pdu_result_failure(CLI_COAP_PREPARE_PDU_ERR_CREATE_OPTLIST);
  }

  if (add_uri_into_pdu(config.uri, config.address, &optlist, request_pdu) ==
      0) {
    coap_log_err("cannot add URI options to PDU\n");
    coap_delete_pdu(request_pdu);
    coap_delete_optlist(optlist);
    return prepare_pdu_result_failure(CLI_COAP_PREPARE_PDU_ERR_ADD_URI_OPTS);
  }
  coap_delete_optlist(optlist);
  return prepare_pdu_ok(request_pdu);
}

struct cli_coap_prepare_pdu_result cli_coap_prepare_get_request(
    const struct cli_coap_session_config config, coap_session_t* coap_session) {
  coap_pdu_t* request_pdu =
      create_request_pdu(coap_session, COAP_REQUEST_CODE_GET);

  // 3. Reutilizamos tus funciones del archivo para inyectar la URI
  // automáticamente
  coap_optlist_t* optlist = NULL;  // Lista vacía, no añadimos payloads de EDHOC
  if (add_uri_into_pdu(config.uri, config.address, &optlist, request_pdu) ==
      0) {
    coap_log_err("cannot add URI options to GET PDU\n");
    coap_delete_pdu(request_pdu);
    if (optlist != NULL)
      coap_delete_optlist(optlist);
    return (struct cli_coap_prepare_pdu_result){
        .status = CLI_COAP_PREPARE_PDU_ERR_ADD_URI_OPTS, .pdu = NULL};
  }

  if (optlist != NULL)
    coap_delete_optlist(optlist);
  return (struct cli_coap_prepare_pdu_result){.status = CLI_COAP_PREPARE_PDU_OK,
                                              .pdu = request_pdu};
}

enum status_coap cli_coap_send_coap_request(coap_session_t* coap_session,
                                            coap_pdu_t* request_pdu) {
  if (coap_send(coap_session, request_pdu) == COAP_INVALID_MID) {
    coap_log_err("cannot send CoAP pdu\n");
    return STATUS_COAP_ERR;
  }

  return STATUS_COAP_OK;
}

enum cli_coap_wait_status cli_coap_wait_for_coap_response(
    coap_context_t* coap_session_context, const coap_session_t* coap_session,
    const bool* have_response) {
  enum { SECONDS_TO_MS = 1000 };

  const u_int16_t maximum_rounded_wait_seconds =
      coap_session_get_default_leisure(coap_session).integer_part + 1;
  int remaining_wait_milliseconds =
      maximum_rounded_wait_seconds * SECONDS_TO_MS;
  int elapsed_milliseconds = 0;
  const u_int16_t max_wait_milliseconds = 1 * SECONDS_TO_MS;
  while (!*have_response) {
    elapsed_milliseconds =
        coap_io_process(coap_session_context, max_wait_milliseconds);
    if (elapsed_milliseconds < 0) {
      coap_log_err("CoAP I/O process failed\n");
      return CLI_COAP_WAIT_IO_ERR;
    }
    if (remaining_wait_milliseconds > 0 &&
        elapsed_milliseconds >= remaining_wait_milliseconds) {
      coap_log_debug("timeout reached\n");
      return CLI_COAP_WAIT_TIMEOUT;
    }
    remaining_wait_milliseconds -= elapsed_milliseconds;
  }

  return CLI_COAP_WAIT_OK;
}
