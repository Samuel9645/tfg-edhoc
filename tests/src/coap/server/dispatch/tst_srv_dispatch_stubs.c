/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 31/03/2026
 * @brief Stubs to use when testing
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
// ReSharper disable CppParameterMayBeConstPtrOrRef
// ReSharper disable CppParameterMayBeConst
// since the real implementations expects non-const pointers to allow output
// parameters
#include "coap/server/dispatch/tst_srv_dispatch_stubs.h"

#include <string.h>

#include "coap/server/internal/parse_edhoc_result_builder.h"

static uint8_t DUMMY_PAYLOAD[] = {0x01, 0x02, 0x03};

cp_srv_parse_edhoc_request_result_t stb_cp_srv_parse_edhoc_request_ok(
    const coap_pdu_t* request,
    const cp_cfg_content_format_edhoc_values_t expected_format) {
  (void)request;
  (void)expected_format;
  return cp_srv_internal_parse_edhoc_ok((com_readonly_buffer_t){
      .bytes = DUMMY_PAYLOAD,
      .length = sizeof(DUMMY_PAYLOAD),
  });
}

cp_srv_parse_edhoc_request_result_t stb_cp_srv_parse_edhoc_request_fail(
    const coap_pdu_t* request,
    const cp_cfg_content_format_edhoc_values_t expected_format) {
  (void)request;
  (void)expected_format;
  return cp_srv_internal_parse_edhoc_failure(CP_SRV_EDH_REQ_ERR_MALFORMED_PDU);
}

static struct edhoc_context dummy_edhoc_context_for_stub = {0};

cp_status_t stb_cp_srv_add_options_success(
    coap_pdu_t* response,
    const cp_cfg_content_format_edhoc_values_t content_format) {
  (void)response;
  (void)content_format;
  return CP_STATUS_SUCCESS;
}

cp_status_t stb_cp_srv_add_options_fail(
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

bool stb_edh_srv_parse_message_1_true(const uint8_t* request_payload,
                                      size_t request_len,
                                      com_readonly_buffer_t* parsed_payload) {
  (void)request_payload;
  (void)request_len;
  if (parsed_payload) {
    static const uint8_t stub_payload[] = {0x01, 0x02};
    parsed_payload->bytes = stub_payload;
    parsed_payload->length = sizeof(stub_payload);
  }
  return true;
}

bool stb_edh_srv_parse_message_1_false(const uint8_t* request_payload,
                                       size_t request_len,
                                       com_readonly_buffer_t* parsed_payload) {
  (void)request_payload;
  (void)request_len;
  (void)parsed_payload;
  return false;
}

bool stb_edh_srv_parse_message_3_true(
    const uint8_t* request_payload, size_t request_len,
    const struct edhoc_context* edhoc_ctx,
    struct edhoc_extracted_fields* extracted_fields) {
  (void)request_payload;
  (void)request_len;
  (void)edhoc_ctx;
  (void)extracted_fields;
  return true;
}

bool stb_edh_srv_parse_message_3_false(
    const uint8_t* request_payload, size_t request_len,
    const struct edhoc_context* edhoc_ctx,
    struct edhoc_extracted_fields* extracted_fields) {
  (void)request_payload;
  (void)request_len;
  (void)edhoc_ctx;
  (void)extracted_fields;
  return false;
}

ehd_srv_message_1_handler_result_t stb_edh_srv_handle_m1_fail(
    const edh_srv_message_1_request_t* request_data,
    com_writable_buffer_t* response_data) {
  (void)request_data;
  (void)response_data;
  return edh_srv_message_1_handler_failure(EDH_SRV_MSG1_HDL_ERR_INVALID_ARGS);
}

ehd_srv_message_1_handler_result_t stb_edh_srv_handle_m1_ok(
    const edh_srv_message_1_request_t* request_data,
    com_writable_buffer_t* response_data) {
  (void)request_data;
  (void)response_data;
  return edh_srv_message_1_handler_ok(&dummy_edhoc_context_for_stub);
}

ehd_srv_message_1_handler_result_t stb_edh_srv_handle_m1_ok_valid_len(
    const edh_srv_message_1_request_t* request_data,
    com_writable_buffer_t* response_data) {
  (void)request_data;
  response_data->length = 10;
  return edh_srv_message_1_handler_ok(&dummy_edhoc_context_for_stub);
}

coap_pdu_code_t stb_cp_srv_process_m1_ok(
    const ehd_srv_message_1_handler_result_t message_1_result,
    coap_session_t* session) {
  (void)message_1_result;
  (void)session;
  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t stb_cp_srv_process_m1_fail(
    const ehd_srv_message_1_handler_result_t message_1_result,
    coap_session_t* session) {
  (void)message_1_result;
  (void)session;
  return COAP_RESPONSE_CODE_BAD_REQUEST;
}

edh_srv_message_3_handler_status_t stb_edh_srv_handle_m3_ok(
    const edh_srv_message_3_request_t* request_data,
    com_writable_buffer_t* response_data) {
  (void)request_data;
  (void)response_data;
  return EDH_MSG3_HDL_OK;
}

edh_srv_message_3_handler_status_t stb_edh_srv_handle_m3_fail(
    const edh_srv_message_3_request_t* request_data,
    com_writable_buffer_t* response_data) {
  (void)request_data;
  (void)response_data;
  return EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_FAILED;
}

coap_pdu_code_t stb_cp_srv_process_m3_ok(
    const edh_srv_message_3_handler_status_t message_3_result) {
  (void)message_3_result;
  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t stb_cp_srv_process_m3_fail(
    const edh_srv_message_3_handler_status_t message_3_result) {
  (void)message_3_result;
  return COAP_RESPONSE_CODE_INTERNAL_ERROR;
}

cp_status_t stb_cp_srv_add_payload_ok(coap_pdu_t* response,
                                      const uint8_t* payload,
                                      size_t payload_len) {
  (void)response;
  (void)payload;
  (void)payload_len;
  return CP_STATUS_SUCCESS;
}

cp_status_t stb_cp_srv_add_payload_fail(coap_pdu_t* response,
                                        const uint8_t* payload,
                                        size_t payload_len) {
  (void)response;
  (void)payload;
  (void)payload_len;
  return CP_STATUS_ERROR;
}
