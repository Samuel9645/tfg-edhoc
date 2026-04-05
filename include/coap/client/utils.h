/**
 * @file
 *
 * @brief Client-side utilities for CoAP.
 *
 * @note This file uses the libcoap library for CoAP protocol handling.
 *
 * @see [libcoap project](https://libcoap.net/)
 *
 * @note Portions of this code are adapted from the libcoap-minimal client.hh
 * example.
 * @see [libcoap-minimal
 * repository](https://github.com/obgm/libcoap-minimal.git)
 *
 * Copyright (C) 2018-2024 Olaf Bergmann <bergmann@tzi.org>
 */

#ifndef COAP_CLIENT_UTILS_H_
#define COAP_CLIENT_UTILS_H_

#include <coap3/coap.h>
#include <stdbool.h>

#include "coap/common/status.h"

// TODO: PARAMETER VALIDATION AND ERROR HANDLING

/**
 * @brief Parse a CoAP URI and resolve the destination endpoint.
 *
 * @param[in] uri_string URI string (for example:
 * coap://localhost:5683/hello).
 * @param[out] parsed_uri Output parsed URI structure.
 * @param[out] destination_address Output resolved socket address.
 * @return CCOM_STATUS_SUCCESS on success, CCOM_ERROR on
 * failure.
 */
enum cp_status cp_cli_parse_and_resolve_coap_uri(
    const char* uri_string, coap_uri_t* parsed_uri,
    coap_address_t* destination_address);

/**
 * @brief Create and configure a CoAP client context/session.
 *
 * @param[in] client_uri Parsed URI used to select transport protocol.
 * @param[in] destination_address Remote destination address.
 * @param[in] response_handler Optional response callback used by libcoap.
 * @param[out] coap_session_context Output created CoAP context.
 * @param[out] coap_session Output created CoAP session.
 * @return CCOM_STATUS_SUCCESS on success, CCOM_ERROR on
 * failure.
 *
 * @note If response_handler is NULL, no callback is registered.
 *
 * @note On failure, both output parameters (context and session) are set to
 * NULL and any allocated resources are freed.
 */
enum cp_status cp_cli_create_coap_session(
    const coap_uri_t* client_uri, const coap_address_t* destination_address,
    coap_response_handler_t response_handler,
    coap_context_t** coap_session_context, coap_session_t** coap_session);

/**
 * @brief Build a POST request PDU and its URI options.
 *
 * @param[in] client_uri Parsed URI.
 * @param[in] destination_address Destination address.
 * @param[in] coap_session Active CoAP session.
 * @param[in] content_format Content format for the EDHOC message.
 *
 * @see [RFC 9528: The Forward Message
 * Flow](https://datatracker.ietf.org/doc/html/rfc9528/#name-the-forward-message-flow)
 * for details on option creation.
 * @return Pointer to created PDU on success, NULL on failure.
 *
 * @note On failure, only the PDU is freed.
 *
 * @warning Cleanups the optlist on both success and failure paths. Callers
 * should not attempt to use or free the optlist after calling this function.
 */
coap_pdu_t* cp_cli_prepare_post_request(
    const coap_uri_t* client_uri, const coap_address_t* destination_address,
    coap_session_t* coap_session,
    enum cp_cfg_content_format_edhoc_values content_format);

/**
 * @brief Send a prepared CoAP request.
 *
 * @param[in] coap_session Active CoAP session.
 * @param[in] request_pdu Prepared request PDU.
 * @return CCOM_STATUS_SUCCESS on success, CCOM_ERROR on
 * failure.
 */
enum cp_status cp_cli_send_coap_request(coap_session_t* coap_session,
                                        coap_pdu_t* request_pdu);

/**
 * @brief Process CoAP I/O until response arrives or timeout is reached.
 *
 * @param[in] coap_session_context Active CoAP context.
 * @param[in] coap_session Active CoAP session.
 * @param[in] have_response Pointer to response flag updated by callback.
 * @return CCOM_STATUS_SUCCESS on success, CCOM_ERROR on
 * failure.
 */
enum cp_status cp_cli_wait_for_coap_response(
    coap_context_t* coap_session_context, const coap_session_t* coap_session,
    const bool* have_response);

#endif  // COAP_CLIENT_UTILS_H_
