#ifndef COAP_SERVER_INTERNAL_dispatch_engine_H_
#define COAP_SERVER_INTERNAL_dispatch_engine_H_

#include <coap3/coap.h>

#include "coap/coap_config.h"
#include "coap/common/com_coap_parse_edhoc_request.h"
#include "coap/common/com_coap_status.h"
#include "coap/server/extract_edhoc_message/srv_coap_extract_m3.h"
#include "coap/server/internal/srv_session.h"
#include "common/com_data_models.h"
#include "edhoc/server/handshake/message_1/srv_m1_responder.h"
#include "edhoc/server/handshake/message_3/srv_m3_responder.h"

typedef struct com_coap_parse_edhoc_request_result (
    *srv_coap_parse_edhoc_request_fn)(
    const coap_pdu_t* request,
    enum config_coap_content_format_edhoc_values expected_format,
    struct com_writable_buffer data_buffer);

typedef enum status_coap (*srv_coap_add_edhoc_response_options_fn)(
    coap_pdu_t* response,
    enum config_coap_content_format_edhoc_values content_format);

typedef bool (*srv_coap_is_message_1_fn)(
    struct com_readonly_buffer parsed_request);

typedef struct srv_coap_extract_message_1_result (
    *srv_coap_extract_message_1_fn)(struct com_readonly_buffer request_buffer,
                                    struct com_writable_buffer error_response);

typedef struct srv_coap_extract_connection_id_result (*srv_coap_extract_cid_fn)(
    struct com_readonly_buffer request_buffer);

typedef bool (*srv_coap_connection_id_is_expected_fn)(
    const struct edhoc_connection_id* extracted_cid,
    const struct edhoc_context* edhoc_ctx);

typedef struct srv_edhoc_message_1_responder_result (
    *srv_edhoc_m1_responder_fn)(struct com_readonly_buffer request_data,
                                struct edhoc_context* edhoc_context,
                                struct com_writable_buffer response_data);

typedef coap_pdu_code_t (*srv_edhoc_m1_process_result_fn)(
    struct srv_edhoc_message_1_responder_result message_1_result);

typedef struct srv_edhoc_message_3_responder_result (
    *srv_edhoc_m3_responder_fn)(
    struct srv_edhoc_message_3_responder_request request_data,
    struct com_writable_buffer response_data);

typedef coap_pdu_code_t (*srv_edhoc_m3_process_result_fn)(
    struct srv_edhoc_message_3_responder_result message_3_result);

typedef enum status_coap (*srv_coap_add_response_payload_fn)(
    coap_pdu_t* response, const uint8_t* payload, size_t payload_len);

typedef enum srv_session_set_status (*srv_session_set_context_fn)(
    const struct edhoc_connection_id* cid, struct edhoc_context* context);

typedef struct srv_session_get_result (*srv_session_get_context_fn)(
    const struct edhoc_connection_id* cid);

typedef enum srv_session_remove_status (*srv_session_remove_context_fn)(
    const struct edhoc_connection_id* cid);

typedef enum status_coap (*srv_oscore_bind_session_fn)(
    coap_context_t* coap_context, struct edhoc_context* edhoc_context);

struct srv_coap_dispatch_deps {
  srv_coap_parse_edhoc_request_fn parse_edhoc_request;
  srv_coap_add_edhoc_response_options_fn add_edhoc_response_options;
  srv_coap_is_message_1_fn is_message_1;
  srv_coap_extract_message_1_fn extract_message_1;
  srv_coap_extract_cid_fn extract_cid;
  srv_coap_connection_id_is_expected_fn connection_id_is_expected;
  srv_edhoc_m1_responder_fn respond_to_message_1;
  srv_edhoc_m1_process_result_fn process_message_1_result;
  srv_edhoc_m3_responder_fn respond_to_message_3;
  srv_edhoc_m3_process_result_fn process_message_3_result;
  srv_coap_add_response_payload_fn add_response_payload;
  srv_session_set_context_fn set_context_by_cid;
  srv_session_get_context_fn get_context_by_cid;
  srv_session_remove_context_fn remove_context_by_cid;
  srv_oscore_bind_session_fn bind_oscore_session;
};

void srv_coap_dispatch_post_with_dependencies(
    const coap_pdu_t* request, struct com_edhoc_parameters edhoc_parameters,
    coap_pdu_t* response, const struct srv_coap_dispatch_deps* deps,
    coap_context_t* context);

#endif  // COAP_SERVER_INTERNAL_dispatch_engine_H_
