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

  if (response_handler != NULL) {
    coap_register_response_handler(*coap_session_context, response_handler);
  }
  return COAP_STATUS_SUCCESS;
}

coap_pdu_t* prepare_coap_post_request(const coap_uri_t* client_uri,
                                      const coap_address_t* destination_address,
                                      coap_session_t* coap_session,
                                      coap_optlist_t* optlist) {
  if (!optlist) {
    coap_log_err("options list is null\n");
    return NULL;
  }

  coap_pdu_t* request_pdu =
      coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_CODE_POST,
                    coap_new_message_id(coap_session),
                    coap_session_max_pdu_size(coap_session));
  if (!request_pdu) {
    coap_log_err("cannot create PDU\n");
    return NULL;
  }

  enum { ADD_PORT_OPTION = 1 };
  if (coap_uri_into_optlist(client_uri, destination_address, &optlist,
                            ADD_PORT_OPTION) == LIBCOAP_ERROR) {
    coap_log_err("cannot create options from URI\n");
    coap_delete_pdu(request_pdu);
    return NULL;
  }

  if (coap_add_optlist_pdu(request_pdu, &optlist) == LIBCOAP_ERROR) {
    coap_log_err("cannot add options to PDU\n");
    coap_delete_pdu(request_pdu);
    return NULL;
  }

  return request_pdu;
}

coap_status_result_t send_coap_request(coap_session_t* coap_session,
                                       coap_pdu_t* request_pdu) {
  if (coap_send(coap_session, request_pdu) == COAP_INVALID_MID) {
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
  int remaining_wait_milliseconds =
      maximum_rounded_wait_seconds * SECONDS_TO_MS;
  int elapsed_milliseconds = 0;
  const u_int16_t max_wait_milliseconds = 1 * SECONDS_TO_MS;
  while (!*have_response) {
    elapsed_milliseconds =
        coap_io_process(coap_session_context, max_wait_milliseconds);
    if (elapsed_milliseconds < 0) {
      coap_log_err("CoAP I/O process failed\n");
      return COAP_STATUS_ERROR;
    }
    if (remaining_wait_milliseconds > 0 &&
        elapsed_milliseconds >= remaining_wait_milliseconds) {
      coap_log_debug("timeout reached\n");
      break;
    }
    remaining_wait_milliseconds -= elapsed_milliseconds;
  }

  return COAP_STATUS_SUCCESS;
}
