#ifndef COAP_COAP_CONFIG_H_
#define COAP_COAP_CONFIG_H_

#include <coap3/coap.h>

/**
 * @brief Bitmask for CoAP block mode configuration to use libcoap for both
 * request and single body data handling.
 */
enum {
  COAP_SHARED_USE_LIBCOAP_FOR_REQUEST_AND_SINGLE_BODY_DATA =
      COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY
};

enum { CBOR_TRUE = 0xF5, MAX_PDU_SIZE = 1024, LIBCOAP_ERROR = 0 };

typedef enum {
  APPLICATION_CID_EDHOC_CBOR_SEQ = 65,
  APPLICATION_EDHOC_CBOR_SEQ = 64
} content_format_edhoc_values_t;

#endif  // COAP_COAP_CONFIG_H_