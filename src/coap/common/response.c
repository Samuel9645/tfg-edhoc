#include "coap/common/response.h"

coap_status_result_t coap_common_add_response_payload(coap_pdu_t* response,
                                                      const uint8_t* payload,
                                                      size_t payload_len) {
  if (!response || !payload || payload_len == 0) {
    coap_log_err("invalid response payload arguments\n");
    return CCOM_ERROR;
  }

  if (!coap_add_data(response, payload_len, payload)) {
    coap_log_err("cannot add data to response\n");
    return CCOM_ERROR;
  }

  return CCOM_STATUS_SUCCESS;
}
