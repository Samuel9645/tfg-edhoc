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

#ifndef COAP_CLIENT_CLI_UTILS_H_
#define COAP_CLIENT_CLI_UTILS_H_

#include <coap3/coap.h>
#include <stdbool.h>

#include "coap/coap_config.h"
#include "coap/common/com_coap_status.h"

// TODO: PARAMETER VALIDATION AND ERROR HANDLING

enum cli_coap_parse_and_resolve_status {
  CLI_COAP_PARSE_AND_RESOLVE_OK,
  CLI_COAP_PARSE_AND_RESOLVE_ERR_INVALID_URI,
  CLI_COAP_PARSE_AND_RESOLVE_ERR_ADDRESS_RESOLUTION
};

struct cli_coap_parse_and_resolve_result {
  const enum cli_coap_parse_and_resolve_status status;
  coap_address_t address;
  coap_uri_t uri;
};

/**
 * @brief Parse a CoAP URI and resolve the destination endpoint.
 *
 * @param[in] uri_string URI string (for example:
 * coap://localhost:5683/hello).
 * @return Struct containing the status of the operation, the parsed URI and the
 * resolved address on success, empty URI and address with the corresponding
 * error status on failure.
 */
struct cli_coap_parse_and_resolve_result cli_coap_parse_and_resolve_coap_uri(
    const char* uri_string);

struct cli_coap_session_config {
  const coap_uri_t* uri;
  const coap_address_t* address;
};

enum cli_coap_create_session_status {
  CLI_COAP_CREATE_SESSION_OK,
  CLI_COAP_CREATE_SESSION_ERR
};

struct cli_coap_create_session_result {
  const enum cli_coap_create_session_status status;
  coap_session_t* session;
};

/**
 * @brief Create and configure a CoAP client session.
 * @param[in] context Active CoAP context.
 * @param [in] config Session configuration parameters
 * @return Struct containing session creation status and the created session on
 * success, NULL session on failure.
 *
 * @note On failure no memory is allocated.
 */
struct cli_coap_create_session_result cli_coap_create_session(
    coap_context_t* context, struct cli_coap_session_config config);

enum cli_coap_prepare_pdu_status {
  CLI_COAP_PREPARE_PDU_OK,
  CLI_COAP_PREPARE_PDU_ERR_CREATE_PDU,
  CLI_COAP_PREPARE_PDU_ERR_CREATE_OPTLIST,
  CLI_COAP_PREPARE_PDU_ERR_ADD_URI_OPTS
};

struct cli_coap_prepare_pdu_result {
  const enum cli_coap_prepare_pdu_status status;
  coap_pdu_t* pdu;
};

/**
 * @brief Build a POST request PDU and its URI options.
 *
 * @param[in] config Session configuration parameters containing URI and
 * destination address.
 * @param[in] coap_session Active CoAP session.
 * @param[in] content_format Content format for the EDHOC message.
 *
 * @see [RFC 9528: The Forward Message
 * Flow](https://datatracker.ietf.org/doc/html/rfc9528/#name-the-forward-message-flow)
 * for details on option creation.
 * @return Struct containing the status of the operation and the prepared PDU on
 * success, NULL pdu pointer on failure
 *
 * @note On failure, only the PDU is freed.
 *
 * @warning Cleanups the optlist on both success and failure paths. Callers
 * should not attempt to use or free the optlist after calling this function.
 */
struct cli_coap_prepare_pdu_result cli_coap_prepare_post_request(
    struct cli_coap_session_config config, coap_session_t* coap_session,
    enum config_coap_content_format_edhoc_values content_format);

/**
 * @brief Send a prepared CoAP request.
 *
 * @param[in] coap_session Active CoAP session.
 * @param[in] request_pdu Prepared request PDU.
 * @return CP_STATUS_SUCCESS on success, CP_STATUS_FAILURE onon
 * failure.
 */
enum status_coap cli_coap_send_coap_request(coap_session_t* coap_session,
                                            coap_pdu_t* request_pdu);

enum cli_coap_wait_status {
  CLI_COAP_WAIT_OK,
  CLI_COAP_WAIT_TIMEOUT,
  CLI_COAP_WAIT_IO_ERR
};

/**
 * @brief Process CoAP I/O until response arrives or timeout is reached.
 *
 * @param[in] coap_session_context Active CoAP context.
 * @param[in] coap_session Active CoAP session.
 * @param[in] have_response Pointer to response flag updated by callback.
 * @return Status code of the wait operation, indicating success, timeout, or
 * I/O error.
 */
enum cli_coap_wait_status cli_coap_wait_for_coap_response(
    coap_context_t* coap_session_context, const coap_session_t* coap_session,
    const bool* have_response);

#endif  // COAP_CLIENT_CLI_UTILS_H_
