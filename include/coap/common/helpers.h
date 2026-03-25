/**
 * @file coap_shared.h
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

#include "coap/coap_config.h"
#include "coap/common/status.h"

/**
 * @brief Resolve a host and port to a CoAP address structure, using scheme
 * hints for protocol selection.
 *
 * @param[in] host Host string to resolve (for example: "localhost").
 * @param[in] port Port number to resolve.
 * @param[in] scheme_hint_bits Scheme hint bits for protocol selection.
 * @param[out] destination_address Output resolved socket address.
 * @return COAP_STATUS_SUCCESS on success, COAP_STATUS_ERROR on failure.
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
coap_status_result_t resolve_address(coap_str_const_t* host, uint16_t port,
                                     int scheme_hint_bits,
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
coap_optlist_t* create_coap_edhoc_optlist(
    content_format_edhoc_values_t content_format);

#endif // COAP_COMMON_HELPERS_H_