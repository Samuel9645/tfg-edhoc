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

#include "coap/server/internal/parse_edhoc_result_builders.h"
#include "edhoc/server/handshake/message_1/internal/srv_m1_handler_result_builders.h"
#include "edhoc/server/handshake/message_1/internal/srv_m1_parser_result_builders.h"
#include "edhoc/server/handshake/message_3/internal/srv_m3_parser_result_builders.h"

static uint8_t DUMMY_PAYLOAD[] = {0x01, 0x02, 0x03};

struct cp_srv_parse_edhoc_request_result stb_cp_srv_parse_edhoc_request_ok(
    const coap_pdu_t* request,
    const enum cp_cfg_content_format_edhoc_values expected_format) {
  (void)request;
  (void)expected_format;
  return cp_srv_internal_parse_edhoc_ok((struct com_readonly_buffer){
      .bytes = DUMMY_PAYLOAD,
      .length = sizeof(DUMMY_PAYLOAD),
  });
}

struct cp_srv_parse_edhoc_request_result stb_cp_srv_parse_edhoc_request_fail(
    const coap_pdu_t* request,
    const enum cp_cfg_content_format_edhoc_values expected_format) {
  (void)request;
  (void)expected_format;
  return cp_srv_internal_parse_edhoc_failure(CP_SRV_EDH_REQ_ERR_MALFORMED_PDU);
}

static struct edhoc_context dummy_edhoc_context_for_stub = {0};

enum cp_status stb_cp_srv_add_options_success(
    coap_pdu_t* response,
    const enum cp_cfg_content_format_edhoc_values content_format) {
  (void)response;
  (void)content_format;
  return CP_STATUS_SUCCESS;
}

enum cp_status stb_cp_srv_add_options_fail(
    coap_pdu_t* response,
    const enum cp_cfg_content_format_edhoc_values content_format) {
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

struct edh_srv_parse_message_1_result stb_edh_srv_parse_message_1_ok(
    const struct com_readonly_buffer request_buffer) {
  (void)request_buffer;
  return cp_srv_internal_parse_message_1_ok((struct com_readonly_buffer){
      .bytes = DUMMY_PAYLOAD,
      .length = sizeof(DUMMY_PAYLOAD),
  });
}

struct edh_srv_parse_message_1_result stb_edh_srv_parse_message_1_failure(
    const struct com_readonly_buffer request_buffer) {
  (void)request_buffer;
  return cp_srv_internal_parse_message_1_failure(
      EDH_SRV_MSG1_PARSE_ERR_PREFIX_EXTRACTION);
}

struct edh_srv_parse_message_3_result stb_edh_srv_parse_message_3_ok(
    struct com_readonly_buffer request_buffer,
    const struct edhoc_context* edhoc_ctx) {
  (void)request_buffer;
  (void)edhoc_ctx;
  return cp_srv_internal_parse_message_3_ok((struct com_readonly_buffer){
      .bytes = DUMMY_PAYLOAD,
      .length = sizeof(DUMMY_PAYLOAD),
  });
}

struct edh_srv_parse_message_3_result stb_edh_srv_parse_message_3_failure(
    struct com_readonly_buffer request_buffer,
    const struct edhoc_context* edhoc_ctx) {
  (void)request_buffer;
  (void)edhoc_ctx;
  return cp_srv_internal_parse_message_3_failure(
      EDH_SRV_MSG3_PARSE_ERR_INVALID_REQUEST_BUFFER);
}

struct edh_srv_message_1_handler_result stb_edh_srv_handle_m1_fail(
    struct edh_srv_message_1_request request_data,
    struct com_writable_buffer* response_data) {
  (void)request_data;
  (void)response_data;
  return edh_srv_message_1_handler_failure(
      EDH_SRV_MSG1_HDL_ERR_NULL_CREDENTIALS);
}

struct edh_srv_message_1_handler_result stb_edh_srv_handle_m1_ok(
    struct edh_srv_message_1_request request_data,
    struct com_writable_buffer* response_data) {
  (void)request_data;
  (void)response_data;
  return edh_srv_message_1_handler_ok(&dummy_edhoc_context_for_stub);
}

struct edh_srv_message_1_handler_result stb_edh_srv_handle_m1_ok_valid_len(
    struct edh_srv_message_1_request request_data,
    struct com_writable_buffer* response_data) {
  (void)request_data;
  response_data->length = 10;
  return edh_srv_message_1_handler_ok(&dummy_edhoc_context_for_stub);
}

coap_pdu_code_t stb_cp_srv_process_m1_ok(
    const struct edh_srv_message_1_handler_result message_1_result,
    coap_session_t* session) {
  (void)message_1_result;
  (void)session;
  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t stb_cp_srv_process_m1_fail(
    const struct edh_srv_message_1_handler_result message_1_result,
    coap_session_t* session) {
  (void)message_1_result;
  (void)session;
  return COAP_RESPONSE_CODE_BAD_REQUEST;
}

enum edh_srv_message_3_handler_status stb_edh_srv_handle_m3_ok(
    const struct edh_srv_message_3_request request_data,
    struct com_writable_buffer* response_data) {
  (void)request_data;
  (void)response_data;
  return EDH_MSG3_HDL_OK;
}

enum edh_srv_message_3_handler_status stb_edh_srv_handle_m3_fail(
    const struct edh_srv_message_3_request request_data,
    struct com_writable_buffer* response_data) {
  (void)request_data;
  (void)response_data;
  return EDH_MSG3_HDL_ERR_MESSAGE_4_COMPOSE_FAILED;
}

coap_pdu_code_t stb_cp_srv_process_m3_ok(
    const enum edh_srv_message_3_handler_status message_3_result) {
  (void)message_3_result;
  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t stb_cp_srv_process_m3_fail(
    const enum edh_srv_message_3_handler_status message_3_result) {
  (void)message_3_result;
  return COAP_RESPONSE_CODE_INTERNAL_ERROR;
}

enum cp_status stb_cp_srv_add_payload_ok(coap_pdu_t* response,
                                         const uint8_t* payload,
                                      size_t payload_len) {
  (void)response;
  (void)payload;
  (void)payload_len;
  return CP_STATUS_SUCCESS;
}

enum cp_status stb_cp_srv_add_payload_fail(coap_pdu_t* response,
                                           const uint8_t* payload,
                                        size_t payload_len) {
  (void)response;
  (void)payload;
  (void)payload_len;
  return CP_STATUS_ERROR;
}
