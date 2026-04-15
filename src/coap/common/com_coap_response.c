#include "coap/common/com_coap_response.h"

#include <stdbool.h>

#include "coap/common/com_coap_helpers.h"

static bool coap_common_response_payload_is_valid(const coap_pdu_t* response,
                                                  const uint8_t* payload,
                                                  const size_t payload_len) {
  return response != NULL && payload != NULL && payload_len > 0;
}

enum status_coap com_coap_add_response_payload(coap_pdu_t* response,
                                               const uint8_t* payload,
                                               const size_t payload_len) {
  if (!coap_common_response_payload_is_valid(response, payload, payload_len)) {
    coap_log_err("invalid response payload arguments\n");
    return STATUS_COAP_ERR;
  }

  if (!coap_add_data(response, payload_len, payload)) {
    coap_log_err("cannot add data to response\n");
    return STATUS_COAP_ERR;
  }
  return STATUS_COAP_OK;
}

enum status_coap com_coap_add_edhoc_response_options(
    coap_pdu_t* response,
    const enum config_coap_content_format_edhoc_values content_format) {
  if (response == NULL) {
    coap_log_err("response pdu is null\n");
    return STATUS_COAP_ERR;
  }

  coap_optlist_t* optlist = com_coap_create_coap_edhoc_optlist(content_format);
  if (optlist == NULL) {
    coap_log_err("cannot create options list\n");
    return STATUS_COAP_ERR;
  }
  if (!coap_add_optlist_pdu(response, &optlist)) {
    coap_log_err("cannot add options to response\n");
    coap_delete_optlist(optlist);
    return STATUS_COAP_ERR;
  }
  coap_delete_optlist(optlist);
  return STATUS_COAP_OK;
}
