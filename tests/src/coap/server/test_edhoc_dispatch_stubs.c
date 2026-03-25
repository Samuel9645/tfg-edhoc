#include "coap/server/test_edhoc_dispatch_stubs.h"

coap_status_result_t extract_fake_payload(
    const coap_pdu_t* request, content_format_edhoc_values_t expected_format,
    const uint8_t** payload, size_t* payload_len) {
  (void)request;
  global_dispatch_trace.validate_request_called = true;
  global_dispatch_trace.expected_request_format = expected_format;

  *payload = global_extracted_payload;
  *payload_len = global_extracted_payload_len;
  return COAP_STATUS_SUCCESS;
}

coap_status_result_t fail_to_extract_payload(
    const coap_pdu_t* request, content_format_edhoc_values_t expected_format,
    const uint8_t** payload, size_t* payload_len) {
  return COAP_STATUS_ERROR;
}

static coap_status_result_t add_edhoc_response_options_with_status(
    coap_pdu_t* response, content_format_edhoc_values_t content_format,
    bool status_to_return) {
  (void)response;
  global_dispatch_trace.add_response_options_called = true;
  global_dispatch_trace.response_options_format = content_format;

  return status_to_return ? COAP_STATUS_SUCCESS : COAP_STATUS_ERROR;
}

coap_status_result_t add_edhoc_response_options_fail(
    coap_pdu_t* response, content_format_edhoc_values_t content_format) {
  return add_edhoc_response_options_with_status(response, content_format,
                                                false);
}

coap_status_result_t add_edhoc_response_options_ok(
    coap_pdu_t* response, content_format_edhoc_values_t content_format) {
  return add_edhoc_response_options_with_status(response, content_format, true);
}

bool is_not_message_1_format(const uint8_t* payload, size_t payload_len) {
  (void)payload;
  (void)payload_len;
  global_dispatch_trace.check_message_1_called = true;
  return false;
}

bool is_not_message_3_format(const uint8_t* request_payload, size_t request_len,
                             const struct edhoc_context* edhoc_ctx,
                             struct edhoc_extracted_fields* extracted_fields) {
  (void)request_payload;
  (void)request_len;
  (void)edhoc_ctx;
  (void)extracted_fields;
  global_dispatch_trace.check_message_3_called = true;
  return false;
}

bool is_message_1_format(const uint8_t* payload, size_t payload_len) {
  global_dispatch_trace.check_message_1_called = true;
  return true;
}

bool is_message_3_format(const uint8_t* request_payload, size_t request_len,
                         const struct edhoc_context* edhoc_ctx,
                         struct edhoc_extracted_fields* extracted_fields) {
  (void)extracted_fields;
  global_dispatch_trace.check_message_3_called = true;
  return true;
}

coap_pdu_code_t handle_message_1_internal_error_with_empty_payload(
    const server_edhoc_message_1_request_data_t* request_data,
    coap_response_data_t* response_data) {
  (void)request_data;
  global_dispatch_trace.handle_message_1_request_called = true;
  *response_data->payload_len = TEST_ZERO_PAYLOAD_LEN;
  return COAP_RESPONSE_CODE_INTERNAL_ERROR;
}

coap_pdu_code_t handle_message_1_changed_with_payload(
    const server_edhoc_message_1_request_data_t* request_data,
    coap_response_data_t* response_data) {
  (void)request_data;
  global_dispatch_trace.handle_message_1_request_called = true;
  response_data->payload[0] = TEST_HANDLER_MESSAGE_BYTE;
  *response_data->payload_len = TEST_SINGLE_PAYLOAD_LEN;
  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t handle_message_1_internal_error_with_payload(
    const server_edhoc_message_1_request_data_t* request_data,
    coap_response_data_t* response_data) {
  (void)request_data;
  global_dispatch_trace.handle_message_1_request_called = true;
  response_data->payload[0] = TEST_ERROR_PAYLOAD_BYTE;
  *response_data->payload_len = TEST_SINGLE_PAYLOAD_LEN;
  return COAP_RESPONSE_CODE_INTERNAL_ERROR;
}

coap_pdu_code_t handle_message_3_changed(
    const server_edhoc_message_3_request_data_t* request_data,
    coap_response_data_t* response_data) {
  (void)request_data;
  global_dispatch_trace.handle_message_3_request_called = true;
  *response_data->payload_len = TEST_ZERO_PAYLOAD_LEN;
  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t handle_message_3_changed_with_prepended_c_r(
    const server_edhoc_message_3_request_data_t* request_data,
    coap_response_data_t* response_data) {
  global_dispatch_trace.handle_message_3_request_called = true;

  response_data->payload[0] = request_data->base_data.request_data.payload[0];
  response_data->payload[1] = TEST_MESSAGE_3_SUFFIX_BYTE;
  *response_data->payload_len = TEST_DOUBLE_PAYLOAD_LEN;

  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t handle_message_3_bad_request_with_error_payload(
    const server_edhoc_message_3_request_data_t* request_data,
    coap_response_data_t* response_data) {
  global_dispatch_trace.handle_message_3_request_called = true;

  response_data->payload[0] = request_data->base_data.request_data.payload[0];
  response_data->payload[1] = TEST_ERROR_PAYLOAD_BYTE;
  *response_data->payload_len = TEST_DOUBLE_PAYLOAD_LEN;

  return COAP_RESPONSE_CODE_BAD_REQUEST;
}

coap_status_result_t add_response_payload_with_status(coap_pdu_t* response,
                                                      const uint8_t* payload,
                                                      size_t payload_len,
                                                      bool status_to_return) {
  (void)response;
  global_dispatch_trace.add_response_payload_called = true;
  global_dispatch_trace.response_payload_len = payload_len;
  if (payload_len > 0) {
    global_dispatch_trace.response_payload_first_byte = payload[0];
  }
  if (payload_len > 1) {
    global_dispatch_trace.response_payload_second_byte = payload[1];
  }

  return status_to_return ? COAP_STATUS_SUCCESS : COAP_STATUS_ERROR;
}

coap_status_result_t add_response_payload_ok(coap_pdu_t* response,
                                             const uint8_t* payload,
                                             size_t payload_len) {
  return add_response_payload_with_status(response, payload, payload_len, true);
}

coap_status_result_t add_response_payload_fail(coap_pdu_t* response,
                                               const uint8_t* payload,
                                               size_t payload_len) {
  return add_response_payload_with_status(response, payload, payload_len,
                                          false);
}

void* get_session_app_data(const coap_session_t* session) {
  const fake_session_t* fake_session = (const fake_session_t*)session;
  return fake_session->app_data;
}

void set_response_code(coap_pdu_t* response, coap_pdu_code_t code) {
  fake_response_t* fake_response = (fake_response_t*)response;
  fake_response->last_code = code;
  fake_response->set_response_code_call_count++;
}

void log_error_noop(const char* message) { (void)message; }
