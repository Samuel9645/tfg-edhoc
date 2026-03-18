/**
 * @file coap_shared.c
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

#include "coap/common/helpers.h"

coap_status_result_t resolve_address(coap_str_const_t* host, uint16_t port,
                                     int scheme_hint_bits,
                                     coap_address_t* destination_address) {
  coap_status_result_t result = COAP_STATUS_ERROR;
  coap_addr_info_t* addr_info;

  addr_info =
      coap_resolve_address_info(host, port, port, port, port, AF_UNSPEC,
                                scheme_hint_bits, COAP_RESOLVE_TYPE_REMOTE);
  if (addr_info) {
    result = COAP_STATUS_SUCCESS;
    *destination_address = addr_info->addr;
  }

  coap_free_address_info(addr_info);
  return result;
}
