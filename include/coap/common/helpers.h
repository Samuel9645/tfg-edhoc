/**
 * @file helpers.h
 *
 * @brief Shared utilities for CoAP client and server implementations.
 *
 * @note This file uses the libcoap library for CoAP protocol handling.
 *
 * @see [libcoap project](https://libcoap.net/)
 *
 * @note The resolve_address function in this file is adapted from the
 * libcoap-minimal common.cc and common.hh files.
 * @see [libcoap-minimal
 * repository](https://github.com/obgm/libcoap-minimal.git)
 *
 * Copyright (C) 2018-2024 Olaf Bergmann <bergmann@tzi.org>
 */

#ifndef COAP_COMMON_HELPERS_H_
#define COAP_COMMON_HELPERS_H_

#include <coap3/coap.h>
#include <stdbool.h>

#include "coap/common/status.h"
#include "coap/config.h"

/**
 * @brief Resolve a host and port to a CoAP address structure, using scheme
 * hints for protocol selection.
 *
 * @param[in] host Host string to resolve (for example: "localhost").
 * @param[in] port Port number to resolve.
 * @param[in] scheme_hint_bits Scheme hint bits for protocol selection.
 * @param[out] destination_address Output resolved socket address.
 * @return CCOM_STATUS_SUCCESS on success, CCOM_ERROR on
 * failure.
 *
 * @note On failure, the output destination_address is not modified.
 *
 * @note This function manages all internal memory used during the DNS
 * resolution process. The caller must provide a valid pointer to an existing
 * `coap_address_t` structure (typically allocated on the stack) to hold the
 * output.
 *
 * @note Inspired by libcoap-minimal common.cc and common.hh files:
 * Copyright (C) 2018-2024 Olaf Bergmann <bergmann@tzi.org>.
 */
cp_status_t cp_com_resolve_address(const coap_str_const_t* host, uint16_t port, int scheme_hint_bits,
                                          coap_address_t* destination_address);

/**
 * @brief Create a CoAP options list with EDHOC-specific options.
 *
 * @param[in] content_format Content format value to include in the options
 * list.
 * @return Pointer to created options list on success, NULL on failure.
 *
 * @see [RFC 9528: The Forward Message
 * Flow](https://datatracker.ietf.org/doc/html/rfc9528/#name-the-forward-message-flow)
 * for details on option creation.
 *
 * @see [RFC 9528: CoAP Content
 * Formats](https://datatracker.ietf.org/doc/html/rfc9528/#name-coap-content-formats-regist)
 * for details on the content format value.
 *
 * @note On failure, any allocated resources are freed.
 */
coap_optlist_t* cp_com_create_coap_edhoc_optlist(
    cp_cfg_content_format_edhoc_values_t content_format);

/**
 * @brief Check if CoAP response code indicates success.
 *
 * @param[in] response_code The CoAP response code to check.
 * @return true if response indicates success (2.04 Changed), false otherwise.
 */
bool cp_com_coap_response_indicates_success(coap_pdu_code_t response_code);

#endif  // COAP_COMMON_HELPERS_H_