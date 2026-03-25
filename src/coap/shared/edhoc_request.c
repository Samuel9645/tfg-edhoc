#include "coap/shared/edhoc_request.h"

#include <edhoc_helpers.h>
#include <edhoc_values.h>
#include <stdbool.h>

coap_status_result_t coap_shared_extract_payload_if_valid_edhoc_request(
    const coap_pdu_t* request, content_format_edhoc_values_t expected_format,
    const uint8_t** payload, size_t* payload_len) {
  if (!request || !payload || !payload_len) {
    coap_log_err("invalid input when validating EDHOC request\n");
    return COAP_STATUS_ERROR;
  }

  coap_opt_iterator_t option_iterator = {0};
  coap_opt_t* content_format_option =
      coap_check_option(request, COAP_OPTION_CONTENT_FORMAT, &option_iterator);
  if (!content_format_option) {
    coap_log_err("missing content format option\n");
    return COAP_STATUS_ERROR;
  }

  uint16_t content_format =
      coap_decode_var_bytes(coap_opt_value(content_format_option),
                            coap_opt_length(content_format_option));
  if (content_format != expected_format) {
    coap_log_err("invalid content format\n");
    return COAP_STATUS_ERROR;
  }

  if (!coap_get_data(request, payload_len, payload)) {
    coap_log_err("cannot get request pdu data\n");
    return COAP_STATUS_ERROR;
  }

  return COAP_STATUS_SUCCESS;
}

bool coap_shared_is_properly_formatted_message_1(const uint8_t* payload,
                                                 size_t payload_len) {
  return payload != NULL && payload_len > 0 && payload[0] == CBOR_TRUE;
}

bool coap_shared_is_properly_formatted_message_3(
    const uint8_t* request_payload, size_t request_len,
    const struct edhoc_context* edhoc_ctx,
    struct edhoc_extracted_fields* extracted_fields) {
  if (!request_payload || request_len == 0 || !edhoc_ctx || !extracted_fields) {
    return false;
  }

  *extracted_fields = (struct edhoc_extracted_fields){
      .buffer = request_payload,
      .buffer_size = request_len,
      .edhoc_message_ptr = request_payload,
      .edhoc_message_size = request_len,
  };

  if (edhoc_extract_connection_id(extracted_fields) != EDHOC_SUCCESS) {
    return false;
  }

  return edhoc_connection_id_equal(&extracted_fields->extracted_conn_id,
                                   &edhoc_ctx->private_cid);
}
