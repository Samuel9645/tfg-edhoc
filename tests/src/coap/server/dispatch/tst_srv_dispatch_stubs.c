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

#include "coap/common/internal/com_parse_edhoc_request_builders.h"
#include "coap/server/extract_edhoc_message/internal/srv_coap_extract_cid_result_builders.h"
#include "coap/server/extract_edhoc_message/internal/srv_coap_extract_m1_result_builders.h"
#include "coap/server/internal/srv_dispatch_engine.h"
#include "edhoc/server/handshake/message_1/internal/srv_m1_responder_result_builders.h"
#include "edhoc/server/handshake/message_3/internal/srv_m3_responder_result_builders.h"

static uint8_t DUMMY_PAYLOAD[] = {0x01, 0x02, 0x03};
static const struct com_readonly_buffer DUMMY_READONLY_BUFFER = {
    .bytes = DUMMY_PAYLOAD,
    .length = sizeof(DUMMY_PAYLOAD),
};
static const struct edhoc_connection_id DUMMY_CONNECTION_ID = {
    .encode_type = EDHOC_CID_TYPE_ONE_BYTE_INTEGER,
    .int_value = 0,
};

struct com_coap_parse_edhoc_request_result stb_srv_coap_parse_edhoc_request_ok(
    const coap_pdu_t* request,
    enum config_coap_content_format_edhoc_values expected_format,
    struct com_writable_buffer data_buffer) {
  (void)request;
  (void)expected_format;
  (void)data_buffer;
  return srv_coap_internal_parse_edhoc_ok(DUMMY_READONLY_BUFFER);
}

struct com_coap_parse_edhoc_request_result
stb_srv_coap_parse_edhoc_request_fail(
    const coap_pdu_t* request,
    enum config_coap_content_format_edhoc_values expected_format,
    struct com_writable_buffer data_buffer) {
  (void)request;
  (void)expected_format;
  (void)data_buffer;
  return srv_coap_internal_parse_edhoc_failure(
      COM_COAP_PARSE_EDHOC_REQ_ERR_UNSUPPORTED_FORMAT);
}

struct srv_coap_extract_connection_id_result stb_srv_coap_extract_cid_ok(
    struct com_readonly_buffer request_buffer) {
  (void)request_buffer;
  return srv_coap_extract_connection_id_ok(DUMMY_CONNECTION_ID,
                                           DUMMY_READONLY_BUFFER);
}

struct srv_coap_extract_connection_id_result stb_srv_coap_extract_cid_failure(
    struct com_readonly_buffer request_buffer) {
  (void)request_buffer;
  return srv_coap_extract_connection_id_failure(
      SRV_COAP_EXTRACT_CID_ERR_EXTRACT);
}

bool stb_srv_coap_connection_id_is_expected_true(
    const struct edhoc_connection_id* extracted_cid,
    const struct edhoc_context* edhoc_ctx) {
  (void)extracted_cid;
  (void)edhoc_ctx;
  return true;
}

bool stb_srv_coap_connection_id_is_expected_false(
    const struct edhoc_connection_id* extracted_cid,
    const struct edhoc_context* edhoc_ctx) {
  (void)extracted_cid;
  (void)edhoc_ctx;
  return false;
}

static struct edhoc_context dummy_edhoc_context_for_stub = {0};

enum status_coap stb_srv_coap_add_options_success(
    coap_pdu_t* response,
    const enum config_coap_content_format_edhoc_values content_format) {
  (void)response;
  (void)content_format;
  return STATUS_COAP_OK;
}

enum status_coap stb_srv_coap_add_options_fail(
    coap_pdu_t* response,
    const enum config_coap_content_format_edhoc_values content_format) {
  (void)response;
  (void)content_format;
  return STATUS_COAP_ERR;
}

bool stb_srv_coap_is_message_1_true(struct com_readonly_buffer parsed_request) {
  (void)parsed_request;
  return true;
}

bool stb_srv_coap_is_message_1_false(
    struct com_readonly_buffer parsed_request) {
  (void)parsed_request;
  return false;
}

enum srv_session_set_status stb_srv_session_set_context_ok(
    const struct edhoc_connection_id* cid, struct edhoc_context* context) {
  (void)cid;
  (void)context;
  return SRV_SESSION_SET_OK;
}

enum srv_session_set_status stb_srv_session_set_context_duplicate_cid(
    const struct edhoc_connection_id* cid, struct edhoc_context* context) {
  (void)cid;
  (void)context;
  return SRV_SESSION_SET_ERR_DUPLICATE_CID;
}

struct srv_session_get_result stb_srv_session_get_context_ok(
    const struct edhoc_connection_id* cid) {
  (void)cid;
  return (struct srv_session_get_result){
      .status = SRV_SESSION_GET_OK, .context = &dummy_edhoc_context_for_stub};
}

struct srv_session_get_result stb_srv_session_get_context_not_found(
    const struct edhoc_connection_id* cid) {
  (void)cid;
  return (struct srv_session_get_result){
      .status = SRV_SESSION_GET_ERR_NOT_FOUND, .context = NULL};
}

enum srv_session_remove_status stb_srv_session_remove_context_ok(
    const struct edhoc_connection_id* cid) {
  (void)cid;
  return SRV_SESSION_REMOVE_OK;
}

enum srv_session_remove_status stb_srv_session_remove_context_not_found(
    const struct edhoc_connection_id* cid) {
  (void)cid;
  return SRV_SESSION_REMOVE_ERR_NOT_FOUND;
}

void* stb_srv_coap_get_session_null(const coap_session_t* session) {
  (void)session;
  return NULL;
}

void* stb_srv_coap_get_session_valid(const coap_session_t* session) {
  (void)session;
  dummy_edhoc_context_for_stub.private_cid = DUMMY_CONNECTION_ID;
  return &dummy_edhoc_context_for_stub;
}

struct srv_coap_extract_message_1_result stb_srv_coap_extract_message_1_ok(
    struct com_readonly_buffer request_buffer,
    struct com_writable_buffer error_response) {
  (void)request_buffer;
  (void)error_response;
  return srv_coap_parse_message_1_ok(DUMMY_READONLY_BUFFER);
}

struct srv_coap_extract_message_1_result
stb_srv_coap_extract_message_1_format_failure(
    struct com_readonly_buffer request_buffer,
    struct com_writable_buffer error_response) {
  (void)request_buffer;
  (void)error_response;
  return srv_coap_parse_message_1_failure(
      SRV_COAP_EXTRACT_MSG1_ERR_UNSUPPORTED_FORWARD_FLOW,
      DUMMY_READONLY_BUFFER);
}

struct srv_edhoc_message_1_responder_result
stb_srv_edhoc_m1_responder_protocol_failure(
    const struct srv_edhoc_message_1_responder_request request_data,
    const struct com_edhoc_parameters edhoc_parameters,
    const struct com_writable_buffer response_data) {
  (void)request_data;
  (void)response_data;
  (void)edhoc_parameters;
  return srv_edhoc_message_1_responder_failure(
      SRV_EDHOC_MSG1_RESPONDER_ERR_INVALID_RESPONSE_BUFFER,
      DUMMY_READONLY_BUFFER);
}

struct srv_edhoc_message_1_responder_result stb_srv_edhoc_m1_responder_ok(
    const struct srv_edhoc_message_1_responder_request request_data,
    const struct com_edhoc_parameters edhoc_parameters,
    const struct com_writable_buffer response_data) {
  (void)request_data;
  (void)response_data;
  (void)edhoc_parameters;
  return srv_edhoc_message_1_responder_ok(&dummy_edhoc_context_for_stub,
                                          DUMMY_READONLY_BUFFER);
}

coap_pdu_code_t stb_srv_coap_process_m1_ok(
    const struct srv_edhoc_message_1_responder_result message_1_result) {
  (void)message_1_result;
  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t stb_srv_coap_process_m1_protocol_failure(
    const struct srv_edhoc_message_1_responder_result message_1_result) {
  (void)message_1_result;
  return COAP_RESPONSE_CODE_BAD_REQUEST;
}

struct srv_edhoc_message_3_responder_result stb_srv_edhoc_m3_responder_ok(
    const struct srv_edhoc_message_3_responder_request request_data,
    struct com_writable_buffer response_data) {
  (void)request_data;
  (void)response_data;
  return srv_edhoc_message_3_responder_ok(DUMMY_READONLY_BUFFER);
}

struct srv_edhoc_message_3_responder_result
stb_srv_edhoc_m3_responder_protocol_failure(
    const struct srv_edhoc_message_3_responder_request request_data,
    struct com_writable_buffer response_data) {
  (void)request_data;
  (void)response_data;
  return srv_edhoc_message_3_responder_failure(
      SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_4_COMPOSE, DUMMY_READONLY_BUFFER);
}

coap_pdu_code_t stb_srv_coap_process_m3_ok(
    const struct srv_edhoc_message_3_responder_result message_3_result) {
  (void)message_3_result;
  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t stb_srv_coap_process_m3_protocol_failure(
    const struct srv_edhoc_message_3_responder_result message_3_result) {
  (void)message_3_result;
  return COAP_RESPONSE_CODE_BAD_REQUEST;
}

enum status_coap stb_srv_oscore_bind_session_failure(
    coap_context_t* coap_context, struct edhoc_context* edhoc_context) {
  (void)coap_context;
  (void)edhoc_context;
  return STATUS_COAP_ERR;
}

enum status_coap stb_srv_oscore_bind_session_ok(
    coap_context_t* coap_context, struct edhoc_context* edhoc_context) {
  (void)coap_context;
  (void)edhoc_context;
  return STATUS_COAP_OK;
}

enum status_coap stb_srv_coap_add_payload_ok(coap_pdu_t* response,
                                             const uint8_t* payload,
                                             size_t payload_len) {
  (void)response;
  (void)payload;
  (void)payload_len;
  return STATUS_COAP_OK;
}

enum status_coap stb_srv_coap_add_payload_failure(coap_pdu_t* response,
                                                  const uint8_t* payload,
                                                  size_t payload_len) {
  (void)response;
  (void)payload;
  (void)payload_len;
  return STATUS_COAP_ERR;
}

struct srv_coap_dispatch_deps test_srv_coap_dispatch_create_base_dependencies(
    void) {
  return (struct srv_coap_dispatch_deps){
      .parse_edhoc_request = stb_srv_coap_parse_edhoc_request_ok,
      .add_edhoc_response_options = stb_srv_coap_add_options_success,
      .is_message_1 = stb_srv_coap_is_message_1_true,
      .get_context_by_cid = stb_srv_session_get_context_ok,
      .remove_context_by_cid = stb_srv_session_remove_context_ok,
      .set_context_by_cid = stb_srv_session_set_context_ok,
      .respond_to_message_1 = stb_srv_edhoc_m1_responder_ok,
      .extract_message_1 = stb_srv_coap_extract_message_1_ok,
      .extract_cid = stb_srv_coap_extract_cid_ok,
      .connection_id_is_expected = stb_srv_coap_connection_id_is_expected_true,
      .process_message_1_result = stb_srv_coap_process_m1_ok,
      .respond_to_message_3 = stb_srv_edhoc_m3_responder_ok,
      .process_message_3_result = stb_srv_coap_process_m3_ok,
      .add_response_payload = stb_srv_coap_add_payload_ok,
      .bind_oscore_session = stb_srv_oscore_bind_session_ok};
}
