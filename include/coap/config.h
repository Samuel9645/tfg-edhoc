#ifndef COAP_CONFIG_H_
#define COAP_CONFIG_H_

/**
 * @brief Bitmask for CoAP block mode configuration to use libcoap for both
 * request and single body data handling.
 */
enum {
  CP_CFG_BLOCK_MODE_LIBCOAP_DEFAULT =
      COAP_BLOCK_USE_LIBCOAP | COAP_BLOCK_SINGLE_BODY
};

enum { CP_CFG_MAX_PDU_SIZE = 1024 };

typedef enum cp_cfg_content_format_edhoc_values {
  CP_CFG_CONTENT_CID_EDHOC = 65,
  CP_CFG_CONTENT_EDHOC = 64
} cp_cfg_content_format_edhoc_values_t;

#endif  // COAP_CONFIG_H_