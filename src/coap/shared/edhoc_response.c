#include "coap/shared/edhoc_response.h"

#include "coap/common/helpers.h"

coap_status_result_t coap_shared_add_edhoc_response_options(
    coap_pdu_t* response, content_format_edhoc_values_t content_format) {
  if (!response) {
    coap_log_err("response pdu is null\n");
    return COAP_STATUS_ERROR;
  }

  coap_optlist_t* optlist = create_coap_edhoc_optlist(content_format);
  if (!optlist) {
    coap_log_err("cannot create options list\n");
    return COAP_STATUS_ERROR;
  }

  if (!coap_add_optlist_pdu(response, &optlist)) {
    coap_log_err("cannot add options to response\n");
    return COAP_STATUS_ERROR;
  }

  return COAP_STATUS_SUCCESS;
}

coap_status_result_t coap_shared_add_response_payload(coap_pdu_t* response,
                                                      const uint8_t* payload,
                                                      size_t payload_len) {
  if (!response || !payload || payload_len == 0) {
    coap_log_err("invalid response payload arguments\n");
    return COAP_STATUS_ERROR;
  }

  if (!coap_add_data(response, payload_len, payload)) {
    coap_log_err("cannot add data to response\n");
    return COAP_STATUS_ERROR;
  }

  return COAP_STATUS_SUCCESS;
}
