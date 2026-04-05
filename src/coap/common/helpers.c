/**
 * @file
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

enum cp_status cp_com_resolve_address(const coap_str_const_t* host,
                                      const uint16_t port,
                                          const int scheme_hint_bits,
                                          coap_address_t* destination_address) {
  enum cp_status result = CP_STATUS_ERROR;

  coap_addr_info_t* addr_info =
      coap_resolve_address_info(host, port, port, port, port, AF_UNSPEC,
                                scheme_hint_bits, COAP_RESOLVE_TYPE_REMOTE);
  if (addr_info) {
    result = CP_STATUS_SUCCESS;
    *destination_address = addr_info->addr;
  }

  coap_free_address_info(addr_info);
  return result;
}

coap_optlist_t* cp_com_create_coap_edhoc_optlist(
    const enum cp_cfg_content_format_edhoc_values content_format) {
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
bool cp_com_coap_response_indicates_success(
    const coap_pdu_code_t response_code) {
  return response_code == COAP_RESPONSE_CODE_CHANGED;
}
