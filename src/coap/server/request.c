/**
 * @file request.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since Tue 31 Mar
 * @brief Definitions of functions to process requests
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/server/request.h"

#include <stdbool.h>

static inline bool coap_server_extract_request_payload_args_are_valid(
    const coap_pdu_t* request, const uint8_t** payload,
    const size_t* payload_len) {
  return (request != NULL) && (payload != NULL) && (payload_len != NULL);
}

coap_status_result_t coap_server_extract_payload_if_valid_edhoc_request(
    const coap_pdu_t* request,
    const content_format_edhoc_values_t expected_format,
    const uint8_t** payload, size_t* payload_len) {
  if (!coap_server_extract_request_payload_args_are_valid(request, payload,
                                                          payload_len)) {
    coap_log_err("invalid input when validating EDHOC request\n");
    return CCOM_ERROR;
  }

  coap_opt_iterator_t option_iterator = {0};
  coap_opt_t* content_format_option =
      coap_check_option(request, COAP_OPTION_CONTENT_FORMAT, &option_iterator);
  if (!content_format_option) {
    coap_log_err("missing content format option\n");
    return CCOM_ERROR;
  }

  const uint16_t content_format =
      coap_decode_var_bytes(coap_opt_value(content_format_option),
                            coap_opt_length(content_format_option));
  if (content_format != expected_format) {
    coap_log_err("invalid content format\n");
    return CCOM_ERROR;
  }

  if (!coap_get_data(request, payload_len, payload)) {
    coap_log_err("cannot get request pdu data\n");
    return CCOM_ERROR;
  }

  return CCOM_STATUS_SUCCESS;
}
