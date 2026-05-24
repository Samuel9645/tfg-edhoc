/**
 * @file
 *
 * @brief Stub implementations used by EDHOC dispatcher seam unit tests.
 */
#ifndef COAP_SERVER_DISPATCH_STUBS_H_
#define COAP_SERVER_DISPATCH_STUBS_H_

#include <coap3/coap.h>

#include "coap/coap_config.h"
#include "coap/common/com_coap_parse_edhoc_request.h"
#include "coap/common/com_coap_status.h"
#include "coap/server/extract_edhoc_message/srv_coap_extract_m3.h"
#include "coap/server/internal/srv_dispatch_engine.h"
#include "coap/server/internal/srv_session.h"
#include "edhoc/server/handshake/message_1/srv_m1_responder.h"
#include "edhoc/server/handshake/message_3/srv_m3_responder.h"

struct com_coap_parse_edhoc_request_result stb_srv_coap_parse_edhoc_request_ok(
    const coap_pdu_t* request,
    enum config_coap_content_format_edhoc_values expected_format,
    struct com_writable_buffer data_buffer);

struct com_coap_parse_edhoc_request_result
stb_srv_coap_parse_edhoc_request_fail(
    const coap_pdu_t* request,
    enum config_coap_content_format_edhoc_values expected_format,
    struct com_writable_buffer data_buffer);

enum status_coap stb_srv_coap_add_options_success(
    coap_pdu_t* response,
    enum config_coap_content_format_edhoc_values content_format);

enum status_coap stb_srv_coap_add_options_fail(
    coap_pdu_t* response,
    enum config_coap_content_format_edhoc_values content_format);

bool stb_srv_coap_is_message_1_true(struct com_readonly_buffer parsed_request);

bool stb_srv_coap_is_message_1_false(struct com_readonly_buffer parsed_request);

enum srv_session_set_status stb_srv_session_set_context_ok(
    const struct edhoc_connection_id* cid, struct edhoc_context* context);

void srv_dispatch_cleanup_last_context(void);

enum srv_session_set_status stb_srv_session_set_context_duplicate_cid(
    const struct edhoc_connection_id* cid, struct edhoc_context* context);

struct srv_session_get_result stb_srv_session_get_context_ok(
    const struct edhoc_connection_id* cid);

struct srv_session_get_result stb_srv_session_get_context_not_found(
    const struct edhoc_connection_id* cid);

enum srv_session_remove_status stb_srv_session_remove_context_ok(
    const struct edhoc_connection_id* cid);

enum srv_session_remove_status stb_srv_session_remove_context_not_found(
    const struct edhoc_connection_id* cid);

struct srv_coap_extract_message_1_result stb_srv_coap_extract_message_1_ok(
    struct com_readonly_buffer request_buffer,
    struct com_writable_buffer error_response);

struct srv_coap_extract_message_1_result
stb_srv_coap_extract_message_1_format_failure(
    struct com_readonly_buffer request_buffer,
    struct com_writable_buffer error_response);

struct srv_coap_extract_connection_id_result stb_srv_coap_extract_cid_ok(
    struct com_readonly_buffer request_buffer);

struct srv_coap_extract_connection_id_result stb_srv_coap_extract_cid_failure(
    struct com_readonly_buffer request_buffer);

bool stb_srv_coap_connection_id_is_expected_true(
    const struct edhoc_connection_id* extracted_cid,
    const struct edhoc_context* edhoc_ctx);

bool stb_srv_coap_connection_id_is_expected_false(
    const struct edhoc_connection_id* extracted_cid,
    const struct edhoc_context* edhoc_ctx);

struct srv_edhoc_message_1_responder_result
stb_srv_edhoc_m1_responder_protocol_failure(
    struct com_readonly_buffer request_data,
    struct edhoc_context* edhoc_context,
    struct com_writable_buffer response_data);

struct srv_edhoc_message_1_responder_result stb_srv_edhoc_m1_responder_ok(
    struct com_readonly_buffer request_data,
    struct edhoc_context* edhoc_context,
    struct com_writable_buffer response_data);

coap_pdu_code_t stb_srv_coap_process_m1_ok(
    struct srv_edhoc_message_1_responder_result message_1_result);

coap_pdu_code_t stb_srv_coap_process_m1_protocol_failure(
    struct srv_edhoc_message_1_responder_result message_1_result);

struct srv_edhoc_message_3_responder_result stb_srv_edhoc_m3_responder_ok(
    struct srv_edhoc_message_3_responder_request request_data,
    struct com_writable_buffer response_data);

struct srv_edhoc_message_3_responder_result
stb_srv_edhoc_m3_responder_protocol_failure(
    struct srv_edhoc_message_3_responder_request request_data,
    struct com_writable_buffer response_data);

coap_pdu_code_t stb_srv_coap_process_m3_ok(
    struct srv_edhoc_message_3_responder_result message_3_result);

coap_pdu_code_t stb_srv_coap_process_m3_protocol_failure(
    struct srv_edhoc_message_3_responder_result message_3_result);

enum status_coap stb_srv_oscore_bind_session_ok(
    coap_context_t* coap_context, struct edhoc_context* edhoc_context);

enum status_coap stb_srv_oscore_bind_session_failure(
    coap_context_t* coap_context, struct edhoc_context* edhoc_context);

enum status_coap stb_srv_coap_add_payload_ok(coap_pdu_t* response,
                                             const uint8_t* payload,
                                             size_t payload_len);

enum status_coap stb_srv_coap_add_payload_failure(coap_pdu_t* response,
                                                  const uint8_t* payload,
                                                  size_t payload_len);

struct srv_coap_dispatch_deps test_srv_coap_dispatch_create_base_dependencies(
    void);

#endif  // COAP_SERVER_DISPATCH_STUBS_H_