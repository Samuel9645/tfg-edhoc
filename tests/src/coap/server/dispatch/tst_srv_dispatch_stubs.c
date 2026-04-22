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

#include "../../../../../src/coap/common/internal/srv_parse_edhoc_request_builders.h"
#include "edhoc/server/handshake/message_1/internal/srv_m1_responder_result_builders.h"
#include "edhoc/server/handshake/message_3/internal/srv_m3_responder_result_builders.h"

static uint8_t DUMMY_PAYLOAD[] = {0x01, 0x02, 0x03};
static const struct com_readonly_buffer DUMMY_READONLY_BUFFER = {
    .bytes = DUMMY_PAYLOAD,
    .length = sizeof(DUMMY_PAYLOAD),
};

struct com_coap_parse_edhoc_request_result stb_srv_coap_parse_edhoc_request_ok(
    const coap_pdu_t* request,
    enum config_coap_content_format_edhoc_values expected_format,
    struct com_writable_buffer* data_buffer) {
  (void)request;
  (void)expected_format;
  (void)data_buffer;
  return srv_coap_internal_parse_edhoc_ok(DUMMY_READONLY_BUFFER);
}

struct com_coap_parse_edhoc_request_result
stb_srv_coap_parse_edhoc_request_fail(
    const coap_pdu_t* request,
    enum config_coap_content_format_edhoc_values expected_format,
    struct com_writable_buffer* data_buffer) {
  (void)request;
  (void)expected_format;
  (void)data_buffer;
  return srv_coap_internal_parse_edhoc_failure(
      COM_COAP_EDH_REQ_ERR_UNSUPPORTED_FORMAT);
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

void* stb_srv_coap_get_session_null(const coap_session_t* session) {
  (void)session;
  return NULL;
}

void* stb_srv_coap_get_session_valid(const coap_session_t* session) {
  (void)session;
  return &dummy_edhoc_context_for_stub;
}

struct srv_edhoc_message_1_responder_result
stb_srv_edhoc_m1_responder_protocol_failure(
    struct srv_edhoc_message_1_responder_request request_data,
    struct com_writable_buffer* response_data) {
  (void)request_data;
  (void)response_data;
  return srv_edhoc_message_1_responder_failure(
      SRV_EDHOC_MSG1_RESPONDER_ERR_INVALID_RESPONSE_BUFFER,
      DUMMY_READONLY_BUFFER);
}

struct srv_edhoc_message_1_responder_result stb_srv_edhoc_m1_responder_ok(
    struct srv_edhoc_message_1_responder_request request_data,
    struct com_writable_buffer* response_data) {
  (void)request_data;
  response_data->length = 10;
  return srv_edhoc_message_1_responder_ok(&dummy_edhoc_context_for_stub,
                                          DUMMY_READONLY_BUFFER);
}

coap_pdu_code_t stb_srv_coap_process_m1_ok(
    const struct srv_edhoc_message_1_responder_result message_1_result,
    coap_session_t* session) {
  (void)message_1_result;
  (void)session;
  return COAP_RESPONSE_CODE_CHANGED;
}

coap_pdu_code_t stb_srv_coap_process_m1_protocol_failure(
    const struct srv_edhoc_message_1_responder_result message_1_result,
    coap_session_t* session) {
  (void)message_1_result;
  (void)session;
  return COAP_RESPONSE_CODE_BAD_REQUEST;
}

struct srv_edhoc_message_3_responder_result stb_srv_edhoc_m3_responder_ok(
    const struct srv_edhoc_message_3_responder_request request_data,
    struct com_writable_buffer* response_data) {
  (void)request_data;
  response_data->length = sizeof(DUMMY_PAYLOAD);
  return srv_edhoc_message_3_responder_ok(DUMMY_READONLY_BUFFER);
}

struct srv_edhoc_message_3_responder_result
stb_srv_edhoc_m3_responder_protocol_failure(
    const struct srv_edhoc_message_3_responder_request request_data,
    struct com_writable_buffer* response_data) {
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
