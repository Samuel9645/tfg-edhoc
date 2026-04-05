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

#include <coap3/coap.h>
#include <stdbool.h>
#include <string.h>

#include "coap/common/helpers.h"
#include "coap/common/status.h"
#include "coap/config.h"

// TODO: PARAMETER VALIDATION AND ERROR HANDLING

enum cp_status cp_cli_parse_and_resolve_coap_uri(
    const char* uri_string, coap_uri_t* parsed_uri,
    coap_address_t* destination_address) {
  if (coap_split_uri((const uint8_t*)uri_string, strlen(uri_string),
                     parsed_uri) != 0) {
    coap_log_warn("cannot parse uri %s\n", uri_string);
    return CP_STATUS_ERROR;
  }

  const uint32_t masked_protocol = 1 << parsed_uri->scheme;
  const enum cp_status resolve_status =
      cp_com_resolve_address(&parsed_uri->host, parsed_uri->port,
                             (int)masked_protocol, destination_address);
  if (resolve_status != CP_STATUS_SUCCESS) {
    coap_log_warn("cannot resolve address %*.*s\n",
                  (int)parsed_uri->host.length, (int)parsed_uri->host.length,
                  (const char*)parsed_uri->host.s);
    return CP_STATUS_ERROR;
  }
  return CP_STATUS_SUCCESS;
}

enum cp_status cp_cli_create_coap_session(
    const coap_uri_t* client_uri, const coap_address_t* destination_address,
    const coap_response_handler_t response_handler,
    coap_context_t** coap_session_context, coap_session_t** coap_session) {
  *coap_session_context = coap_new_context(NULL);
  if (*coap_session_context == NULL) {
    coap_log_err("cannot create libcoap context\n");
    return CP_STATUS_ERROR;
  }

  coap_context_set_block_mode(
      *coap_session_context,
                              CP_CFG_BLOCK_MODE_LIBCOAP_DEFAULT);

  const coap_proto_t protocol = client_uri->scheme == COAP_URI_SCHEME_COAP_TCP
                                    ? COAP_PROTO_TCP
                                    : COAP_PROTO_UDP;
  const coap_address_t* local_interface_address = NULL;
  *coap_session =
      coap_new_client_session(*coap_session_context, local_interface_address,
                              destination_address, protocol);
  if (*coap_session == NULL) {
    coap_log_err("cannot create client session\n");
    return CP_STATUS_ERROR;
  }

  if (response_handler != NULL) {
    coap_register_response_handler(*coap_session_context, response_handler);
  }
  return CP_STATUS_SUCCESS;
}

/**
 * @brief Create a CoAP POST request PDU with the given URI options.
 * @param[in] coap_session Active CoAP session used to create the PDU.
 * @return Pointer to the created PDU, or NULL on error.
 */
static coap_pdu_t* create_post_request_pdu(coap_session_t* coap_session) {
  return coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_CODE_POST,
                       coap_new_message_id(coap_session),
                       coap_session_max_pdu_size(coap_session));
}

/**
 * @brief Adds URI options to the given optlist based on the provided client URI
 * and destination address.
 * @param[in] client_uri Parsed URI containing the options to add.
 * @param[in] destination_address Destination address associated with the URI,
 * used for port option.
 * @param[out] optlist Pointer to the optlist to add options to.
 * @return 1 on success, 0 if error.
 */
static int add_uri_into_optlist(const coap_uri_t* client_uri,
                                const coap_address_t* destination_address,
                                coap_optlist_t** optlist) {
  enum { CCU_ADD_PORT_OPTION = 1 };
  return coap_uri_into_optlist(client_uri, destination_address, optlist,
                               CCU_ADD_PORT_OPTION);
}

/**
 * @brief Adds URI options to the given CoAP request PDU based on the provided
 * client URI and destination address.
 * @param[in] client_uri Parsed URI containing the options to add.
 * @param[in] destination_address Destination address associated with the URI,
 * used for port option.
 * @param[in] options_list CoAP optlist containing the URI options to add.
 * @param[out] request_pdu CoAP request PDU to add options to.
 * @return 1 on success, 0 if error.
 */
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

coap_pdu_t* cp_cli_prepare_post_request(
    const coap_uri_t* client_uri, const coap_address_t* destination_address,
    coap_session_t* coap_session,
    const enum cp_cfg_content_format_edhoc_values content_format) {
  coap_pdu_t* request_pdu = create_post_request_pdu(coap_session);
  if (!request_pdu) {
    coap_log_err("cannot create PDU\n");
    return NULL;
  }
  coap_optlist_t* optlist = cp_com_create_coap_edhoc_optlist(content_format);
  if (!optlist) {
    coap_log_err("cannot create options list\n");
    coap_delete_pdu(request_pdu);
    return NULL;
  }
  enum { CCU_ERROR_INDICATOR = 0 };
  if (add_uri_into_pdu(client_uri, destination_address, &optlist,
                       request_pdu) == CCU_ERROR_INDICATOR) {
    coap_log_err("cannot add URI options to PDU\n");
    coap_delete_pdu(request_pdu);
    coap_delete_optlist(optlist);
    return NULL;
  }
  coap_delete_optlist(optlist);
  return request_pdu;
}

enum cp_status cp_cli_send_coap_request(coap_session_t* coap_session,
                                        coap_pdu_t* request_pdu) {
  if (coap_send(coap_session, request_pdu) == COAP_INVALID_MID) {
    coap_log_err("cannot send CoAP pdu\n");
    return CP_STATUS_ERROR;
  }

  return CP_STATUS_SUCCESS;
}

enum cp_status cp_cli_wait_for_coap_response(
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
      return CP_STATUS_ERROR;
    }
    if (remaining_wait_milliseconds > 0 &&
        elapsed_milliseconds >= remaining_wait_milliseconds) {
      coap_log_debug("timeout reached\n");
      break;
    }
    remaining_wait_milliseconds -= elapsed_milliseconds;
  }

  return CP_STATUS_SUCCESS;
}
