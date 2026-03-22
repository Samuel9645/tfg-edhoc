#include "coap/client/log_edhoc_error_response.h"

#include <edhoc.h>

void coap_client_log_received_edhoc_error_response(
    const coap_pdu_code_t response_code, const uint8_t* payload,
    const size_t payload_len) {
  enum edhoc_error_code edhoc_error_code = EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;
  struct edhoc_error_info edhoc_error_info = {0};

  if (edhoc_message_error_process(payload, payload_len, &edhoc_error_code,
                                  &edhoc_error_info) == EDHOC_SUCCESS) {
    coap_log_err("received EDHOC error response: %d.%02d, error code: %d\n",
                 response_code >> 5, response_code & 0x1F, edhoc_error_code);
  } else {
    coap_log_err(
        "received CoAP error response: %d.%02d with unparseable EDHOC error "
        "payload\n",
        response_code >> 5, response_code & 0x1F);
  }
}
