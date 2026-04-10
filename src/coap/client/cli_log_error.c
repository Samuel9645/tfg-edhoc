#include "coap/client/cli_log_error.h"

#include <edhoc.h>

void cli_coap_log_received_edhoc_error_response(
    const coap_pdu_code_t response_code,
    const struct com_readonly_buffer error) {
  enum edhoc_error_code edhoc_error_code = EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;
  struct edhoc_error_info edhoc_error_info = {0};

  if (edhoc_message_error_process(error.bytes, error.length, &edhoc_error_code,
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
