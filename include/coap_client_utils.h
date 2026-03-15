#ifndef COAP_CLIENT_UTILS_H
#define COAP_CLIENT_UTILS_H

#include <stdbool.h>

#include "coap3/coap.h"

typedef enum CoapUtilsResult {
  COAP_UTILS_SUCCESS = 0,
  COAP_UTILS_ERROR = -1
} CoapUtilsResult;

/**
 * Parse a CoAP URI and resolve the destination endpoint.
 *
 * @param[in] uri_string Input URI string (for example:
 * coap://localhost:5683/hello).
 * @param[out] parsed_uri Output parsed URI structure.
 * @param[out] destination_address Output resolved socket address.
 * @param[out] is_mcast Output multicast flag (non-zero if multicast
 * destination).
 * @return COAP_UTILS_SUCCESS on success, COAP_UTILS_ERROR on failure.
 */
CoapUtilsResult parse_and_resolve_coap_uri(const char* uri_string,
                                           coap_uri_t* parsed_uri,
                                           coap_address_t* destination_address,
                                           int* is_mcast);

/**
 * Create and configure a CoAP client context/session.
 *
 * @param[in] client_uri Input parsed URI used to select transport protocol.
 * @param[in] destination_address Input remote destination address.
 * @param[in] response_handler Input response callback used by libcoap.
 * @param[out] coap_session_context Output created CoAP context.
 * @param[out] coap_session Output created CoAP session.
 * @return COAP_UTILS_SUCCESS on success, COAP_UTILS_ERROR on failure.
 */
CoapUtilsResult create_coap_client_session(
    const coap_uri_t* client_uri, const coap_address_t* destination_address,
    coap_response_handler_t response_handler,
    coap_context_t** coap_session_context, coap_session_t** coap_session);

/**
 * Build a GET request PDU and its URI options.
 *
 * @param[in] client_uri Input parsed URI.
 * @param[in] destination_address Input destination address.
 * @param[in] is_mcast Input multicast flag.
 * @param[in] coap_session Input active CoAP session.
 * @param[out] protocol_data_unit Output created request PDU.
 * @param[out] optlist Output created option list.
 * @return COAP_UTILS_SUCCESS on success, COAP_UTILS_ERROR on failure.
 */
CoapUtilsResult prepare_coap_get_request(
    const coap_uri_t* client_uri, const coap_address_t* destination_address,
    int is_mcast, coap_session_t* coap_session, coap_pdu_t** protocol_data_unit,
    coap_optlist_t** optlist);

/**
 * Send a prepared CoAP request.
 *
 * @param[in] coap_session Input active CoAP session.
 * @param[in] protocol_data_unit Input prepared request PDU.
 * @return COAP_UTILS_SUCCESS on success, COAP_UTILS_ERROR on failure.
 */
CoapUtilsResult send_coap_request(coap_session_t* coap_session,
                                  coap_pdu_t* protocol_data_unit);

/**
 * Process CoAP I/O until response arrives or timeout is reached.
 *
 * @param[in] coap_session_context Input active CoAP context.
 * @param[in] coap_session Input active CoAP session.
 * @param[in] have_response Input pointer to response flag updated by callback.
 * @param[in] is_mcast Input multicast flag.
 * @return COAP_UTILS_SUCCESS on success, COAP_UTILS_ERROR on failure.
 */
CoapUtilsResult wait_for_coap_response(coap_context_t* coap_session_context,
                                       coap_session_t* coap_session,
                                       const bool* have_response, int is_mcast);

#endif
