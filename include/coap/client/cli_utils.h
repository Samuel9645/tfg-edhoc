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
#include "coap/config.h"

// TODO: PARAMETER VALIDATION AND ERROR HANDLING

enum cp_parse_and_resolve_status {
  CP_PARSE_AND_RESOLVE_OK,
  CP_PARSE_AND_RESOLVE_ERR_INVALID_URI,
  CP_PARSE_AND_RESOLVE_ERR_ADDRESS_RESOLUTION
};

struct cp_cli_parse_and_resolve_result {
  enum cp_parse_and_resolve_status status;
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
struct cp_cli_parse_and_resolve_result cp_cli_parse_and_resolve_coap_uri(
    const char* uri_string);

struct cp_cli_session_config {
  const coap_uri_t* uri;
  const coap_address_t* address;
};

enum cp_cli_create_session_status {
  CP_CLI_CREATE_SESSION_OK,
  CP_CLI_CREATE_SESSION_ERR
};

struct cp_cli_create_session_result {
  enum cp_cli_create_session_status status;
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
struct cp_cli_create_session_result cp_cli_create_session(
    coap_context_t* context, struct cp_cli_session_config config);

enum cp_cli_prepare_pdu_status {
  CP_CLI_PREPARE_PDU_OK,
  CP_CLI_PREPARE_PDU_ERR_CREATE_PDU,
  CP_CLI_PREPARE_PDU_ERR_CREATE_OPTLIST,
  CP_CLI_PREPARE_PDU_ERR_ADD_URI_OPTS
};

struct cp_cli_prepare_pdu_result {
  enum cp_cli_prepare_pdu_status status;
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
struct cp_cli_prepare_pdu_result cp_cli_prepare_post_request(
    struct cp_cli_session_config config, coap_session_t* coap_session,
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

enum cp_cli_wait_status {
  CP_CLI_WAIT_OK,
  CP_CLI_WAIT_TIMEOUT,
  CP_CLI_WAIT_IO_ERR
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
enum cp_cli_wait_status cp_cli_wait_for_coap_response(
    coap_context_t* coap_session_context, const coap_session_t* coap_session,
    const bool* have_response);

#endif  // COAP_CLIENT_UTILS_H_
