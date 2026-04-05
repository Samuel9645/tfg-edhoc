/**
 * @file
 *
 * @brief Stub implementations used by EDHOC dispatcher seam unit tests.
 */
#ifndef COAP_SERVER_DISPATCH_STUBS_H_
#define COAP_SERVER_DISPATCH_STUBS_H_

#include <coap3/coap.h>
#include <edhoc_helpers.h>

#include "coap/common/status.h"
#include "coap/config.h"
#include "coap/server/srv_parse_edhoc_request.h"
#include "edhoc/server/handshake/message_1/srv_m1_handler.h"
#include "edhoc/server/handshake/message_1/srv_m1_parser.h"
#include "edhoc/server/handshake/message_3/srv_m3_handler.h"
#include "edhoc/server/handshake/message_3/srv_m3_parser.h"

cp_srv_parse_edhoc_request_result_t stb_cp_srv_parse_edhoc_request_ok(
    const coap_pdu_t* request,
    cp_cfg_content_format_edhoc_values_t expected_format);

cp_srv_parse_edhoc_request_result_t stb_cp_srv_parse_edhoc_request_fail(
    const coap_pdu_t* request,
    cp_cfg_content_format_edhoc_values_t expected_format);

cp_status_t stb_cp_srv_add_options_success(
    coap_pdu_t* response, cp_cfg_content_format_edhoc_values_t content_format);

cp_status_t stb_cp_srv_add_options_fail(
    coap_pdu_t* response, cp_cfg_content_format_edhoc_values_t content_format);

void* stb_cp_srv_get_session_null(const coap_session_t* session);

void* stb_cp_srv_get_session_valid(const coap_session_t* session);

edh_srv_parse_message_1_result_t stb_edh_srv_parse_message_1_ok(
    com_readonly_buffer_t request_buffer);

edh_srv_parse_message_1_result_t stb_edh_srv_parse_message_1_failure(
    com_readonly_buffer_t request_buffer);

edh_srv_parse_message_3_result_t stb_edh_srv_parse_message_3_ok(
    com_readonly_buffer_t request_buffer,
    const struct edhoc_context* edhoc_ctx);

edh_srv_parse_message_3_result_t stb_edh_srv_parse_message_3_failure(
    com_readonly_buffer_t request_buffer,
    const struct edhoc_context* edhoc_ctx);

ehd_srv_message_1_handler_result_t stb_edh_srv_handle_m1_fail(
    const edh_srv_message_1_request_t* request_data,
    com_writable_buffer_t* response_data);

ehd_srv_message_1_handler_result_t stb_edh_srv_handle_m1_ok(
    const edh_srv_message_1_request_t* request_data,
    com_writable_buffer_t* response_data);

ehd_srv_message_1_handler_result_t stb_edh_srv_handle_m1_ok_valid_len(
    const edh_srv_message_1_request_t* request_data,
    com_writable_buffer_t* response_data);

coap_pdu_code_t stb_cp_srv_process_m1_ok(
    ehd_srv_message_1_handler_result_t message_1_result,
    coap_session_t* session);

coap_pdu_code_t stb_cp_srv_process_m1_fail(
    ehd_srv_message_1_handler_result_t message_1_result,
    coap_session_t* session);

edh_srv_message_3_handler_status_t stb_edh_srv_handle_m3_ok(
    edh_srv_message_3_request_t request_data,
    com_writable_buffer_t* response_data);

edh_srv_message_3_handler_status_t stb_edh_srv_handle_m3_fail(
    edh_srv_message_3_request_t request_data,
    com_writable_buffer_t* response_data);

coap_pdu_code_t stb_cp_srv_process_m3_ok(
    edh_srv_message_3_handler_status_t message_3_result);

coap_pdu_code_t stb_cp_srv_process_m3_fail(
    edh_srv_message_3_handler_status_t message_3_result);

cp_status_t stb_cp_srv_add_payload_ok(coap_pdu_t* response,
                                      const uint8_t* payload,
                                      size_t payload_len);

cp_status_t stb_cp_srv_add_payload_fail(coap_pdu_t* response,
                                        const uint8_t* payload,
                                        size_t payload_len);

#endif  // COAP_SERVER_DISPATCH_STUBS_H_