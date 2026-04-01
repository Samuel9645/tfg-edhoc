/**
 * @file stubs.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Stubs to use when testing
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
// ReSharper disable CppParameterMayBeConstPtrOrRef
// ReSharper disable CppParameterMayBeConst
// since the real implementations expects non-const pointers to allow output
// parameters
#include "coap/server/dispatch/stubs.h"

cp_status_result_t stb_cp_srv_extract_payload_success(
    const coap_pdu_t* request,
    const cp_cfg_content_format_edhoc_values_t expected_format,
    const uint8_t** payload,
    size_t* payload_len) {  // NOLINT(*-non-const-parameter)
  (void)request;
  (void)expected_format;
  (void)payload;
  (void)payload_len;
  return CP_STATUS_SUCCESS;
}

cp_status_result_t stb_cp_srv_extract_payload_fail(
    const coap_pdu_t* request,
    const cp_cfg_content_format_edhoc_values_t expected_format,
    const uint8_t** payload,
    size_t* payload_len) {  // NOLINT(*-non-const-parameter)
  (void)request;
  (void)expected_format;
  (void)payload;
  (void)payload_len;
  return CP_STATUS_ERROR;
}

static struct edhoc_context dummy_edhoc_context_for_stub = {0};

cp_status_result_t stb_cp_srv_add_options_success(
    coap_pdu_t* response,
    const cp_cfg_content_format_edhoc_values_t content_format) {
  (void)response;
  (void)content_format;
  return CP_STATUS_SUCCESS;
}

cp_status_result_t stb_cp_srv_add_options_fail(
    coap_pdu_t* response,
    const cp_cfg_content_format_edhoc_values_t content_format) {
  (void)response;
  (void)content_format;
  return CP_STATUS_ERROR;
}

void* stb_cp_srv_get_session_null(const coap_session_t* session) {
  (void)session;
  return NULL;
}

void* stb_cp_srv_get_session_valid(const coap_session_t* session) {
  (void)session;
  return &dummy_edhoc_context_for_stub;
}

bool stb_edh_srv_check_is_m1_true(const uint8_t* payload,
                                  const size_t payload_len) {
  (void)payload;
  (void)payload_len;
  return true;
}

bool stb_edh_srv_check_is_m1_false(const uint8_t* payload, const size_t payload_len) {
  (void)payload;
  (void)payload_len;
  return false;
}

edh_srv_hnd_m1_result_t test_edh_srv_handle_m1_success(
    const edh_srv_hnd_m1_request_t* request_data,
    com_response_buffer_t* response_data) {
  (void)request_data;
  (void)response_data;
  return edh_srv_hnd_m1_failure(EDH_SERV_HND_M1_ERR_INVALID_ARGS);
}

edh_srv_hnd_m1_result_t stb_edh_srv_handle_m1_ok(
    const edh_srv_hnd_m1_request_t* request_data,
    com_response_buffer_t* response_data) {
  (void)request_data;
  (void)response_data;
  return edh_srv_hnd_m1_ok(&dummy_edhoc_context_for_stub);
}

edh_srv_hnd_m1_result_t stb_edh_srv_handle_m1_ok_valid_len(
    const edh_srv_hnd_m1_request_t* request_data,
    com_response_buffer_t* response_data) {
  (void)request_data;
  response_data->payload_length = 10;
  return edh_srv_hnd_m1_ok(&dummy_edhoc_context_for_stub);
}

coap_pdu_code_t stb_cp_srv_process_m1_ok(
    const edh_srv_hnd_m1_result_t message_1_result, coap_session_t* session) {
  (void)message_1_result;
  (void)session;
  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t stb_cp_srv_process_m1_fail(
    const edh_srv_hnd_m1_result_t message_1_result, coap_session_t* session) {
  (void)message_1_result;
  (void)session;
  return COAP_RESPONSE_CODE_BAD_REQUEST;
}

bool stb_edh_srv_check_m3_true(
    const uint8_t* request_payload, const size_t request_len,
                       const struct edhoc_context* edhoc_ctx,
                       struct edhoc_extracted_fields* extracted_fields) {
  (void)request_payload;
  (void)request_len;
  (void)edhoc_ctx;
  (void)extracted_fields;
  return true;
}

bool stb_edh_srv_check_m3_false(
    const uint8_t* request_payload,
                           const size_t request_len,
                           const struct edhoc_context* edhoc_ctx,
                           struct edhoc_extracted_fields* extracted_fields) {
  (void)request_payload;
  (void)request_len;
  (void)edhoc_ctx;
  (void)extracted_fields;
  return false;
}

edh_srv_hnd_m3_result_t stb_edh_srv_handle_m3_ok(
    const edh_srv_hnd_m3_request_data_t* request_data,
    com_response_buffer_t* response_data) {
  (void)request_data;
  (void)response_data;
  return EDH_SRV_HND_M3_OK;
}

edh_srv_hnd_m3_result_t stb_edh_srv_handle_m3_fail(
    const edh_srv_hnd_m3_request_data_t* request_data,
    com_response_buffer_t* response_data) {
  (void)request_data;
  (void)response_data;
  return EDH_SRV_HND_M3_ERR_MESSAGE_4_COMPOSE_FAILED;
}

coap_pdu_code_t stb_cp_srv_process_m3_ok(
    const edh_srv_hnd_m3_result_t message_3_result) {
  (void)message_3_result;
  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t stb_cp_srv_process_m3_fail(
    const edh_srv_hnd_m3_result_t message_3_result) {
  (void)message_3_result;
  return COAP_RESPONSE_CODE_INTERNAL_ERROR;
}

cp_status_result_t stb_cp_srv_add_payload_ok(
    coap_pdu_t* response, const uint8_t* payload, const size_t payload_len) {
  (void)response;
  (void)payload;
  (void)payload_len;
  return CP_STATUS_SUCCESS;
}

cp_status_result_t stb_cp_srv_add_payload_fail(coap_pdu_t* response,
                                               const uint8_t* payload,
                                                    const size_t payload_len) {
  (void)response;
  (void)payload;
  (void)payload_len;
  return CP_STATUS_ERROR;
}