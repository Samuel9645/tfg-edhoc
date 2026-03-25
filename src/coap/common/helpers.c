/**
 * @file helpers.c
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

coap_optlist_t* create_coap_edhoc_optlist(
    content_format_edhoc_values_t content_format) {
  enum { CREATE_PORT_HOST_OPTION = 1 };
  coap_optlist_t* optlist = NULL;
  enum { ENCODE_BUFFER_SIZE = 4 };
  uint8_t content_format_value[ENCODE_BUFFER_SIZE];
  const unsigned int compressed_length = coap_encode_var_safe(
      content_format_value, sizeof(content_format_value), content_format);
  if (compressed_length == 0) {
    coap_log_err("cannot encode content format\n");
    return NULL;
  }
  coap_optlist_t* edhoc_optlist = coap_new_optlist(
      COAP_OPTION_CONTENT_FORMAT, compressed_length, content_format_value);
  if (!edhoc_optlist) {
    coap_log_err("cannot create EDHOC options\n");
    return NULL;
  }
  if (coap_insert_optlist(&optlist, edhoc_optlist) == 0) {
    coap_log_err("cannot add EDHOC options to list\n");
    coap_delete_optlist(edhoc_optlist);
    return NULL;
  }
  return optlist;
}
