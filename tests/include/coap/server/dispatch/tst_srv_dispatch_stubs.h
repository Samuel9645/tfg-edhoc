/**
 * @file
 *
 * @brief Stub implementations used by EDHOC dispatcher seam unit tests.
 */
#ifndef COAP_SERVER_DISPATCH_STUBS_H_
#define COAP_SERVER_DISPATCH_STUBS_H_

#include <coap3/coap.h>

#include "coap/coap_config.h"
#include "coap/common/com_coap_status.h"
#include "coap/server/srv_parse_edhoc_request.h"
#include "edhoc/server/handshake/message_1/srv_m1_parser.h"
#include "edhoc/server/handshake/message_1/srv_m1_responder.h"
#include "edhoc/server/handshake/message_3/srv_m3_parser.h"
#include "edhoc/server/handshake/message_3/srv_m3_responder.h"

struct srv_coap_parse_edhoc_request_result stb_srv_coap_parse_edhoc_request_ok(
    const coap_pdu_t* request,
    enum config_coap_content_format_edhoc_values expected_format,
    struct com_writable_buffer* data_buffer);

struct srv_coap_parse_edhoc_request_result
stb_srv_coap_parse_edhoc_request_fail(
    const coap_pdu_t* request,
    enum config_coap_content_format_edhoc_values expected_format,
    struct com_writable_buffer* data_buffer);

enum status_coap stb_srv_coap_add_options_success(
    coap_pdu_t* response,
    enum config_coap_content_format_edhoc_values content_format);

enum status_coap stb_srv_coap_add_options_fail(
    coap_pdu_t* response,
    enum config_coap_content_format_edhoc_values content_format);

void* stb_srv_coap_get_session_null(const coap_session_t* session);

void* stb_srv_coap_get_session_valid(const coap_session_t* session);

struct srv_edhoc_parse_message_1_result stb_srv_edhoc_parse_m1_ok(
    struct com_readonly_buffer request_buffer);

struct srv_edhoc_parse_message_1_result stb_srv_edhoc_parse_m1_protocol_failure(
    struct com_readonly_buffer request_buffer);

struct srv_edhoc_parse_message_3_result stb_srv_edhoc_parse_message_3_ok(
    struct com_readonly_buffer request_buffer,
    const struct edhoc_context* edhoc_ctx);

struct srv_edhoc_parse_message_3_result stb_srv_edhoc_parse_m3_protocol_failure(
    struct com_readonly_buffer request_buffer,
    const struct edhoc_context* edhoc_ctx);

struct srv_edhoc_message_1_responder_result
stb_srv_edhoc_handle_m1_protocol_failure(
    struct srv_edhoc_message_1_request request_data,
    struct com_writable_buffer* response_data);

struct srv_edhoc_message_1_responder_result stb_srv_edhoc_handle_m1_ok(
    struct srv_edhoc_message_1_request request_data,
    struct com_writable_buffer* response_data);

coap_pdu_code_t stb_srv_coap_process_m1_ok(
    struct srv_edhoc_message_1_responder_result message_1_result,
    coap_session_t* session);

coap_pdu_code_t stb_srv_coap_process_m1_protocol_failure(
    struct srv_edhoc_message_1_responder_result message_1_result,
    coap_session_t* session);

struct srv_edhoc_message_3_responder_result stb_srv_edhoc_handle_m3_ok(
    struct srv_edhoc_message_3_request request_data,
    struct com_writable_buffer* response_data);

struct srv_edhoc_message_3_responder_result
stb_srv_edhoc_handle_m3_protocol_failure(
    struct srv_edhoc_message_3_request request_data,
    struct com_writable_buffer* response_data);

coap_pdu_code_t stb_srv_coap_process_m3_ok(
    struct srv_edhoc_message_3_responder_result message_3_result);

coap_pdu_code_t stb_srv_coap_process_m3_failure(
    struct srv_edhoc_message_3_responder_result message_3_result);

enum status_coap stb_srv_coap_add_payload_ok(coap_pdu_t* response,
                                             const uint8_t* payload,
                                             size_t payload_len);

enum status_coap stb_srv_coap_add_payload_failure(coap_pdu_t* response,
                                                  const uint8_t* payload,
                                                  size_t payload_len);

#endif  // COAP_SERVER_DISPATCH_STUBS_H_