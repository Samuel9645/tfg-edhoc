#ifndef COAP_COMMON_CONFIG_H_
#define COAP_COMMON_CONFIG_H_

#include <coap3/coap.h>

/**
 * @brief Bitmask for CoAP block mode configuration to use libcoap for both
 * request and single body data handling.
 */
enum {
  COAP_SHARED_USE_LIBCOAP_FOR_REQUEST_AND_SINGLE_BODY_DATA =
      COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY
};

#endif  // COAP_COMMON_CONFIG_H_