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

#include "coap/common/status.h"

/**
 * @brief Bitmask for CoAP block mode configuration to use libcoap for both
 * request and single body data handling.
 */
enum {
  COAP_SHARED_USE_LIBCOAP_FOR_REQUEST_AND_SINGLE_BODY_DATA =
      COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY
};

/**
 * @brief Cleanup function to end a CoAP session and free associated resources.
 * @param[in] options CoAP options list to delete (can be NULL).
 * @param[in] session CoAP session to release (can be NULL).
 * @param[in] context CoAP context to free (can be NULL).
 * @return -1 to indicate session termination.
 *
 * @note This function is designed to be used as a common exit point for CoAP
 * client and server implementations, ensuring consistent cleanup of resources.
 * The caller can pass NULL for any parameter that does not need to be cleaned
 * up.
 *
 * @warning This function calls coap_cleanup(), which should only be called once
 * when the application is finished using libcoap. This function must not
 * be called multiple times or while other CoAP operations are still in
 * progress.
 */
int end_coap_session(coap_optlist_t* options, coap_session_t* session,
                     coap_context_t* context);

/**
 * @brief Resolve a host and port to a CoAP address structure, using scheme
 * hints for protocol selection.
 * @param[in] host Host string to resolve (for example: "localhost").
 * @param[in] port Port number to resolve.
 * @param[in] scheme_hint_bits Scheme hint bits for protocol selection.
 * @param[out] destination_address Output resolved socket address.
 * @return COAP_STATUS_SUCCESS on success, COAP_STATUS_ERROR on failure.
 *
 * @note On failure, the output destination_address is not modified
 *
 * @note This function manages all internal memory used during the DNS
 * resolution process. The caller must provide a valid pointer to an existing
 * `coap_address_t` structure (typically allocated on the stack) to hold the
 * output.
 *
 *
 * Inspired by libcoap-minimal common.cc and common.hh files:
 * Copyright (C) 2018-2024 Olaf Bergmann <bergmann@tzi.org>
 */
CoapStatusResult resolve_address(coap_str_const_t* host, uint16_t port,
                                 int scheme_hint_bits,
                                 coap_address_t* destination_address);

#endif  // COAP_COMMON_HELPERS_H_