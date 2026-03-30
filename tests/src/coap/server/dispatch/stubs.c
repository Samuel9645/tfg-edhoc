#include "coap/server/dispatch/stubs.h"

coap_status_result_t successful_extract_payload_stub(
    const coap_pdu_t* request, content_format_edhoc_values_t expected_format,
    const uint8_t** payload, size_t* payload_len) {
  (void)request;
  (void)expected_format;
  (void)payload;
  (void)payload_len;
  return COAP_STATUS_SUCCESS;
}

coap_status_result_t failed_extract_payload_stub(
    const coap_pdu_t* request, content_format_edhoc_values_t expected_format,
    const uint8_t** payload, size_t* payload_len) {
  (void)request;
  (void)expected_format;
  (void)payload;
  (void)payload_len;
  return COAP_STATUS_ERROR;
}

static struct edhoc_context dummy_edhoc_context_for_stub = {0};

coap_status_result_t successful_add_response_options_stub(
    coap_pdu_t* response, content_format_edhoc_values_t content_format) {
  (void)response;
  (void)content_format;
  return COAP_STATUS_SUCCESS;
}

coap_status_result_t failed_add_response_options_stub(
    coap_pdu_t* response, content_format_edhoc_values_t content_format) {
  (void)response;
  (void)content_format;
  return COAP_STATUS_ERROR;
}

void* get_null_session_app_data_stub(const coap_session_t* session) {
  (void)session;
  return NULL;
}

void* get_non_null_session_app_data_stub(const coap_session_t* session) {
  (void)session;
  return &dummy_edhoc_context_for_stub;
}

bool is_message_1_stub(const uint8_t* payload, size_t payload_len) {
  (void)payload;
  (void)payload_len;
  return true;
}

bool is_not_message_1_stub(const uint8_t* payload, size_t payload_len) {
  (void)payload;
  (void)payload_len;
  return false;
}

coap_pdu_code_t successful_handle_message_1_stub(
    const edhoc_server_message_1_request_data_t* request_data,
    coap_response_data_t* response_data) {
  (void)request_data;
  (void)response_data;
  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t failed_handle_message_1_stub(
    const edhoc_server_message_1_request_data_t* request_data,
    coap_response_data_t* response_data) {
  (void)request_data;
  (void)response_data;
  return COAP_RESPONSE_CODE_INTERNAL_ERROR;
}

bool is_message_3_stub(const uint8_t* request_payload, size_t request_len,
                       const struct edhoc_context* edhoc_ctx,
                       struct edhoc_extracted_fields* extracted_fields) {
  (void)request_payload;
  (void)request_len;
  (void)edhoc_ctx;
  (void)extracted_fields;
  return true;
}

bool is_not_message_3_stub(const uint8_t* request_payload, size_t request_len,
                           const struct edhoc_context* edhoc_ctx,
                           struct edhoc_extracted_fields* extracted_fields) {
  (void)request_payload;
  (void)request_len;
  (void)edhoc_ctx;
  (void)extracted_fields;
  return false;
}

coap_pdu_code_t successful_handle_message_3_stub(
    const edhoc_server_message_3_request_data_t* request_data,
    coap_response_data_t* response_data) {
  (void)request_data;
  (void)response_data;
  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t failed_handle_message_3_stub(
    const edhoc_server_message_3_request_data_t* request_data,
    coap_response_data_t* response_data) {
  (void)request_data;
  (void)response_data;
  return COAP_RESPONSE_CODE_INTERNAL_ERROR;
}

coap_status_result_t successful_add_response_payload_stub(
    coap_pdu_t* response, const uint8_t* payload, size_t payload_len) {
  (void)response;
  (void)payload;
  (void)payload_len;
  return COAP_STATUS_SUCCESS;
}

coap_status_result_t failed_add_response_payload_stub(coap_pdu_t* response,
                                                      const uint8_t* payload,
                                                      size_t payload_len) {
  (void)response;
  (void)payload;
  (void)payload_len;
  return COAP_STATUS_ERROR;
}