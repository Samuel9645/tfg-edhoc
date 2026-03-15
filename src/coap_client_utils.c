#include "coap_client_utils.h"

#include <string.h>

#include "coap_shared.h"
#include "minimal_coap_common.h"

CoapUtilsResult parse_and_resolve_coap_uri(const char* uri_string,
                                           coap_uri_t* parsed_uri,
                                           coap_address_t* destination_address,
                                           int* is_mcast) {
  int result = coap_split_uri((const uint8_t*)uri_string, strlen(uri_string),
                              parsed_uri);
  if (result != 0) {
    coap_log_warn("Failed to parse uri %s\n", uri_string);
    return COAP_UTILS_ERROR;
  }

  const uint32_t masked_protocol = 1 << parsed_uri->scheme;
  result = resolve_address(&parsed_uri->host, parsed_uri->port,
                           destination_address, masked_protocol);
  if (result <= 0) {
    coap_log_warn("Failed to resolve address %*.*s\n",
                  (int)parsed_uri->host.length, (int)parsed_uri->host.length,
                  (const char*)parsed_uri->host.s);
    return COAP_UTILS_ERROR;
  }

  *is_mcast = coap_is_mcast(destination_address);
  return COAP_UTILS_SUCCESS;
}

CoapUtilsResult create_coap_client_session(
    const coap_uri_t* client_uri, const coap_address_t* destination_address,
    coap_response_handler_t response_handler,
    coap_context_t** coap_session_context, coap_session_t** coap_session) {
  *coap_session_context = coap_new_context(NULL);
  if (!*coap_session_context) {
    coap_log_emerg("cannot create libcoap context\n");
    return COAP_UTILS_ERROR;
  }

  coap_context_set_block_mode(
      *coap_session_context,
      COAP_SHARED_USE_LIBCOAP_FOR_REQUEST_AND_SINGLE_BODY_DATA);

  const coap_proto_t protocol = client_uri->scheme == COAP_URI_SCHEME_COAP_TCP
                                    ? COAP_PROTO_TCP
                                    : COAP_PROTO_UDP;
  *coap_session = coap_new_client_session(*coap_session_context, NULL,
                                          destination_address, protocol);
  if (!*coap_session) {
    coap_log_emerg("cannot create client session\n");
    return COAP_UTILS_ERROR;
  }

  coap_register_response_handler(*coap_session_context, response_handler);
  return COAP_UTILS_SUCCESS;
}

CoapUtilsResult prepare_coap_get_request(
    const coap_uri_t* client_uri, const coap_address_t* destination_address,
    int is_mcast, coap_session_t* coap_session, coap_pdu_t** protocol_data_unit,
    coap_optlist_t** optlist) {
  *protocol_data_unit =
      coap_pdu_init(is_mcast ? COAP_MESSAGE_NON : COAP_MESSAGE_CON,
                    COAP_REQUEST_CODE_GET, coap_new_message_id(coap_session),
                    coap_session_max_pdu_size(coap_session));
  if (!*protocol_data_unit) {
    coap_log_emerg("cannot create PDU\n");
    return COAP_UTILS_ERROR;
  }

  enum { ADD_PORT_OPTION = 1, BUFFER_SIZE = 1000 };
  unsigned char scratch[BUFFER_SIZE] = {0};
  int result = coap_uri_into_options(client_uri, destination_address, optlist,
                                     ADD_PORT_OPTION, scratch, sizeof(scratch));
  if (result != 0) {
    coap_log_err("Failed to create options\n");
    return COAP_UTILS_ERROR;
  }

  if (*optlist) {
    result = coap_add_optlist_pdu(*protocol_data_unit, optlist);
    if (result != 1) {
      coap_log_err("Failed to add options to PDU\n");
      return COAP_UTILS_ERROR;
    }
  }

  return COAP_UTILS_SUCCESS;
}

CoapUtilsResult send_coap_request(coap_session_t* coap_session,
                                  coap_pdu_t* protocol_data_unit) {
  if (coap_send(coap_session, protocol_data_unit) == COAP_INVALID_MID) {
    coap_log_err("cannot send CoAP pdu\n");
    return COAP_UTILS_ERROR;
  }

  return COAP_UTILS_SUCCESS;
}

CoapUtilsResult wait_for_coap_response(coap_context_t* coap_session_context,
                                       coap_session_t* coap_session,
                                       const bool* have_response,
                                       int is_mcast) {
  enum { TIMEOUT_MS = 1000 };
  int wait_ms =
      (coap_session_get_default_leisure(coap_session).integer_part + 1) *
      TIMEOUT_MS;
  int miliseconds_spent_on_function = 0;
  while (!*have_response || is_mcast) {
    miliseconds_spent_on_function =
        coap_io_process(coap_session_context, TIMEOUT_MS);
    if (miliseconds_spent_on_function < 0) {
      coap_log_err("CoAP I/O process failed\n");
      return COAP_UTILS_ERROR;
    }
    if (wait_ms < 0 || miliseconds_spent_on_function >= wait_ms) {
      coap_log_warn("No response received within timeout\n");
      return COAP_UTILS_ERROR;
    }
    wait_ms -= miliseconds_spent_on_function;
  }

  return COAP_UTILS_SUCCESS;
}
