#ifndef COAP_COAP_CONFIG_H_
#define COAP_COAP_CONFIG_H_

#include <coap3/coap.h>

/**
 * @brief Bitmask for CoAP block mode configuration to use libcoap for both
 * request and single body data handling.
 */
enum {
  CONFIG_COAP_BLOCK_MODE_LIBCOAP_DEFAULT =
      COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY
};

enum { CONFIG_COAP_MAX_PDU_SIZE = 1024 };

enum config_coap_content_format_edhoc_values {
  CONFIG_COAP_CONTENT_CID_EDHOC = 65,
  CONFIG_COAP_CONTENT_EDHOC = 64
};

#endif  // COAP_COAP_CONFIG_H_