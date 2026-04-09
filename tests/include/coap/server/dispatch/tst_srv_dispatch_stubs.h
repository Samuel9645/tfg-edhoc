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

struct cp_srv_parse_edhoc_request_result stb_cp_srv_parse_edhoc_request_ok(
    const coap_pdu_t* request,
    enum cp_cfg_content_format_edhoc_values expected_format);

struct cp_srv_parse_edhoc_request_result stb_cp_srv_parse_edhoc_request_fail(
    const coap_pdu_t* request,
    enum cp_cfg_content_format_edhoc_values expected_format);

enum cp_status stb_cp_srv_add_options_success(
    coap_pdu_t* response,
    enum cp_cfg_content_format_edhoc_values content_format);

enum cp_status stb_cp_srv_add_options_fail(
    coap_pdu_t* response,
    enum cp_cfg_content_format_edhoc_values content_format);

void* stb_cp_srv_get_session_null(const coap_session_t* session);

void* stb_cp_srv_get_session_valid(const coap_session_t* session);

struct edh_srv_parse_message_1_result stb_edh_srv_parse_message_1_ok(
    struct com_readonly_buffer request_buffer);

struct edh_srv_parse_message_1_result stb_edh_srv_parse_message_1_failure(
    struct com_readonly_buffer request_buffer);

struct edh_srv_parse_message_3_result stb_edh_srv_parse_message_3_ok(
    struct com_readonly_buffer request_buffer,
    const struct edhoc_context* edhoc_ctx);

struct edh_srv_parse_message_3_result stb_edh_srv_parse_message_3_failure(
    struct com_readonly_buffer request_buffer,
    const struct edhoc_context* edhoc_ctx);

struct edh_srv_message_1_handler_result stb_edh_srv_handle_m1_fail(
    struct edh_srv_message_1_request request_data,
    struct com_writable_buffer* response_data);

struct edh_srv_message_1_handler_result stb_edh_srv_handle_m1_ok(
    struct edh_srv_message_1_request request_data,
    struct com_writable_buffer* response_data);

struct edh_srv_message_1_handler_result stb_edh_srv_handle_m1_ok_valid_len(
    struct edh_srv_message_1_request request_data,
    struct com_writable_buffer* response_data);

coap_pdu_code_t stb_cp_srv_process_m1_ok(
    struct edh_srv_message_1_handler_result message_1_result,
    coap_session_t* session);

coap_pdu_code_t stb_cp_srv_process_m1_fail(
    struct edh_srv_message_1_handler_result message_1_result,
    coap_session_t* session);

enum edh_srv_message_3_handler_status stb_edh_srv_handle_m3_ok(
    struct edh_srv_message_3_request request_data,
    struct com_writable_buffer* response_data);

enum edh_srv_message_3_handler_status stb_edh_srv_handle_m3_fail(
    struct edh_srv_message_3_request request_data,
    struct com_writable_buffer* response_data);

coap_pdu_code_t stb_cp_srv_process_m3_ok(
    enum edh_srv_message_3_handler_status message_3_result);

coap_pdu_code_t stb_cp_srv_process_m3_fail(
    enum edh_srv_message_3_handler_status message_3_result);

enum cp_status stb_cp_srv_add_payload_ok(coap_pdu_t* response,
                                         const uint8_t* payload,
                                         size_t payload_len);

enum cp_status stb_cp_srv_add_payload_fail(coap_pdu_t* response,
                                           const uint8_t* payload,
                                           size_t payload_len);

#endif  // COAP_SERVER_DISPATCH_STUBS_H_