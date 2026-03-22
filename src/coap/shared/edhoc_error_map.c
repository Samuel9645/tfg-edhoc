#include "coap/shared/edhoc_error_map.h"

coap_pdu_code_t coap_shared_map_edhoc_failure_to_response(
    struct edhoc_context* edhoc_ctx, const char* operation_label,
    coap_shared_edhoc_failure_type_t failure_type, int edhoc_api_result,
    coap_pdu_t* response, coap_response_data_t* response_data) {
  coap_pdu_code_t mapped_response_code =
      failure_type == COAP_SHARED_EDHOC_PROTOCOL_ERROR
          ? COAP_RESPONSE_CODE_BAD_REQUEST
          : COAP_RESPONSE_CODE_INTERNAL_ERROR;

  if (!response) {
    return mapped_response_code;
  }

  if (!edhoc_ctx || !response_data) {
    coap_log_err("cannot map EDHOC error without context or response data\n");
    coap_pdu_set_code(response, COAP_RESPONSE_CODE_INTERNAL_ERROR);
    return COAP_RESPONSE_CODE_INTERNAL_ERROR;
  }

  enum edhoc_error_code edhoc_error_code = EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;
  if (edhoc_error_get_code(edhoc_ctx, &edhoc_error_code) != EDHOC_SUCCESS) {
    coap_log_err(
        "cannot get EDHOC-level error code, using UNSPECIFIED_ERROR\n");
  }

  coap_log_err("cannot %s, libedhoc ret: %d, edhoc error code: %d\n",
               operation_label, edhoc_api_result, edhoc_error_code);

  if (edhoc_message_error_compose(response_data->payload,
                                  response_data->payload_capacity,
                                  response_data->payload_len, edhoc_error_code,
                                  NULL) != EDHOC_SUCCESS) {
    coap_log_err(
        "cannot compose EDHOC error message for client, using empty payload\n");
    *response_data->payload_len = 0;
  }

  coap_pdu_set_code(response, mapped_response_code);
  return mapped_response_code;
}
