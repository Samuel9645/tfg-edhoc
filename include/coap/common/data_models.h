#ifndef COAP_COMMON_DATA_MODELS_H_
#define COAP_COMMON_DATA_MODELS_H_

#include <coap3/coap.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Shared CoAP session data used by client/server helpers.
 */
typedef struct {
  /** Active CoAP context used for I/O polling and callbacks. */
  coap_context_t* context;

  /** Active CoAP session bound to the remote peer. */
  coap_session_t* session;
} coap_session_data_t;

/**
 * @brief Shared endpoint data for CoAP requests.
 */
typedef struct {
  /** Parsed URI that identifies the remote EDHOC resource. */
  const coap_uri_t* uri;

  /** Resolved destination socket address associated with uri. */
  const coap_address_t* destination;
} coap_endpoint_data_t;

/**
 * @brief Generic request payload descriptor.
 */
typedef struct {
  /** Pointer to request bytes to send or process. */
  const uint8_t* payload;

  /** Length of payload in bytes. */
  size_t payload_len;
} coap_request_data_t;

/**
 * @brief Generic response buffer descriptor.
 */
typedef struct {
  /** Caller-provided buffer where response bytes are written. */
  uint8_t* payload;

  /** Capacity of payload buffer in bytes. */
  size_t payload_capacity;

  /** Output number of bytes written to payload buffer. */
  size_t* payload_len;
} coap_response_data_t;

#endif  // COAP_COMMON_DATA_MODELS_H_