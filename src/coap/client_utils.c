/**
 * @file coap_client_utils.c
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

#include <coap3/coap.h>
#include <stdbool.h>
#include <string.h>

#include "coap/common/config.h"
#include "coap/common/helpers.h"
#include "coap/common/status.h"

// TODO: PARAMETER VALIDATION AND ERROR HANDLING

coap_status_result_t parse_and_resolve_coap_uri(
    const char* uri_string, coap_uri_t* parsed_uri,
    coap_address_t* destination_address) {
  if (coap_split_uri((const uint8_t*)uri_string, strlen(uri_string),
                     parsed_uri) != 0) {
    coap_log_warn("cannot parse uri %s\n", uri_string);
    return COAP_STATUS_ERROR;
  }

  const uint32_t masked_protocol = 1 << parsed_uri->scheme;
  coap_status_result_t resolve_status =
      resolve_address(&parsed_uri->host, parsed_uri->port, masked_protocol,
                      destination_address);
  if (resolve_status != COAP_STATUS_SUCCESS) {
    coap_log_warn("cannot resolve address %*.*s\n",
                  (int)parsed_uri->host.length, (int)parsed_uri->host.length,
                  (const char*)parsed_uri->host.s);
    return COAP_STATUS_ERROR;
  }
  return COAP_STATUS_SUCCESS;
}

coap_status_result_t create_coap_client_session(
    const coap_uri_t* client_uri, const coap_address_t* destination_address,
    coap_response_handler_t response_handler,
    coap_context_t** coap_session_context, coap_session_t** coap_session) {
  *coap_session_context = coap_new_context(NULL);
  if (!*coap_session_context) {
    coap_log_err("cannot create libcoap context\n");
    return COAP_STATUS_ERROR;
  }

  coap_context_set_block_mode(
      *coap_session_context,
      COAP_SHARED_USE_LIBCOAP_FOR_REQUEST_AND_SINGLE_BODY_DATA);

  const coap_proto_t protocol = client_uri->scheme == COAP_URI_SCHEME_COAP_TCP
                                    ? COAP_PROTO_TCP
                                    : COAP_PROTO_UDP;
  coap_address_t* local_interface_address = NULL;
  *coap_session =
      coap_new_client_session(*coap_session_context, local_interface_address,
                              destination_address, protocol);
  if (!*coap_session) {
    coap_log_err("cannot create client session\n");
    return COAP_STATUS_ERROR;
  }

  coap_register_response_handler(*coap_session_context, response_handler);
  return COAP_STATUS_SUCCESS;
}

coap_optlist_t* create_coap_edhoc_optlist(void) {
  enum { CREATE_PORT_HOST_OPTION = 1 };
  coap_optlist_t* optlist = NULL;
  enum { APPLICATION_CID_EDHOC_CBOR_SEQ = 65 };
  enum { ENCODE_BUFFER_SIZE = 4 };
  uint8_t content_format_value[ENCODE_BUFFER_SIZE];
  const unsigned int compressed_length =
      coap_encode_var_safe(content_format_value, sizeof(content_format_value),
                           APPLICATION_CID_EDHOC_CBOR_SEQ);
  if (compressed_length == 0) {
    coap_log_err("cannot encode content format\n");
    return NULL;
  }
  coap_optlist_t* edhoc_optlist = coap_new_optlist(
      COAP_OPTION_CONTENT_FORMAT, compressed_length, content_format_value);
  if (!edhoc_optlist) {
    coap_log_err("cannot create EDHOC options\n");
    return NULL;
  }
  if (coap_insert_optlist(&optlist, edhoc_optlist) == 0) {
    coap_log_err("cannot add EDHOC options to list\n");
    coap_delete_optlist(edhoc_optlist);
    return NULL;
  }
  return optlist;
}

coap_pdu_t* prepare_coap_post_request(const coap_uri_t* client_uri,
                                      const coap_address_t* destination_address,
                                      coap_session_t* coap_session,
                                      coap_optlist_t* optlist) {
  if (!optlist) {
    coap_log_err("options list is null\n");
    return NULL;
  }

  coap_pdu_t* protocol_data_unit =
      coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_CODE_POST,
                    coap_new_message_id(coap_session),
                    coap_session_max_pdu_size(coap_session));
  if (!protocol_data_unit) {
    coap_log_err("cannot create PDU\n");
    return NULL;
  }

  enum { ADD_PORT_OPTION = 1, LIBCOAP_ERROR = 0 };
  if (coap_uri_into_optlist(client_uri, destination_address, &optlist,
                            ADD_PORT_OPTION) == LIBCOAP_ERROR) {
    coap_log_err("cannot create options from URI\n");
    coap_delete_pdu(protocol_data_unit);
    return NULL;
  }

  if (coap_add_optlist_pdu(protocol_data_unit, &optlist) == LIBCOAP_ERROR) {
    coap_log_err("cannot add options to PDU\n");
    coap_delete_pdu(protocol_data_unit);
    return NULL;
  }

  return protocol_data_unit;
}

coap_status_result_t send_coap_request(coap_session_t* coap_session,
                                       coap_pdu_t* protocol_data_unit) {
  if (coap_send(coap_session, protocol_data_unit) == COAP_INVALID_MID) {
    coap_log_err("cannot send CoAP pdu\n");
    return COAP_STATUS_ERROR;
  }

  return COAP_STATUS_SUCCESS;
}

coap_status_result_t wait_for_coap_response(
    coap_context_t* coap_session_context, coap_session_t* coap_session,
    const bool* have_response) {
  enum { SECONDS_TO_MS = 1000 };
  const u_int16_t maximum_rounded_wait_seconds =
      coap_session_get_default_leisure(coap_session).integer_part + 1;
  int wait_miliseconds = maximum_rounded_wait_seconds * SECONDS_TO_MS;
  int miliseconds_spent_on_function = 0;
  const u_int16_t MAX_WAIT_MILISECONDS = 1 * SECONDS_TO_MS;
  while (!*have_response) {
    miliseconds_spent_on_function =
        coap_io_process(coap_session_context, MAX_WAIT_MILISECONDS);
    if (miliseconds_spent_on_function < 0) {
      coap_log_err("CoAP I/O process failed\n");
      return COAP_STATUS_ERROR;
    }
    if (wait_miliseconds > 0 &&
        miliseconds_spent_on_function >= wait_miliseconds) {
      coap_log_debug("timeout reached\n");
      break;
    }
    wait_miliseconds -= miliseconds_spent_on_function;
  }

  return COAP_STATUS_SUCCESS;
}
