#include "coap/shared/edhoc_error_map.h"

void coap_shared_map_edhoc_failure_to_response(struct edhoc_context* edhoc_ctx,
                                               const char* operation_label,
                                               bool protocol_error,
                                               coap_pdu_t* response) {
  if (!response) {
    return;
  }

  if (!edhoc_ctx) {
    coap_log_err("cannot map EDHOC error without context\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  enum edhoc_error_code err = EDHOC_ERROR_GENERIC_ERROR;
  if (edhoc_error_get_code(edhoc_ctx, &err) != EDHOC_SUCCESS) {
    coap_log_err("cannot get error code from EDHOC context\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return;
  }

  coap_log_err("cannot %s, error code: %d\n", operation_label, err);
  coap_pdu_set_code(response, protocol_error
                                  ? COAP_RESPONSE_CODE_BAD_REQUEST
                                  : COAP_RESPONSE_CODE_INTERNAL_ERROR);
}
