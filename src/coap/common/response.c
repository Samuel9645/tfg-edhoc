#include "coap/common/response.h"

#include <stdbool.h>

static bool coap_common_response_payload_is_valid(const coap_pdu_t* response,
                                                  const uint8_t* payload,
                                                  const size_t payload_len) {
  return (response != NULL) && (payload != NULL) && (payload_len > 0);
}

coap_status_result_t coap_common_add_response_payload(coap_pdu_t* response, const uint8_t* payload, const size_t payload_len) {
  if (!coap_common_response_payload_is_valid(response, payload, payload_len)) {
    coap_log_err("invalid response payload arguments\n");
    return CCOM_ERROR;
  }

  if (!coap_add_data(response, payload_len, payload)) {
    coap_log_err("cannot add data to response\n");
    return CCOM_ERROR;
  }

  return CCOM_STATUS_SUCCESS;
}
