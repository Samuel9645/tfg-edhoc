/**
 * @file test_edhoc_dispatch_stubs.h
 *
 * @brief Stub implementations used by EDHOC dispatcher seam unit tests.
 *
 * Each function updates trace state and returns deterministic results to drive
 * success and error paths through the dispatcher.
 */
#ifndef COAP_SERVER_DISPATCH_STUBS_H_
#define COAP_SERVER_DISPATCH_STUBS_H_

#include <coap3/coap.h>
#include <edhoc_helpers.h>

#include "coap/coap_config.h"
#include "coap/common/status.h"
#include "edhoc/server/handshake.h"
#include "edhoc/server/handshake/message_1_handler.h"

coap_status_result_t successful_extract_payload_stub(
    const coap_pdu_t* request, content_format_edhoc_values_t expected_format,
    const uint8_t** payload, size_t* payload_len);

coap_status_result_t failed_extract_payload_stub(
    const coap_pdu_t* request, content_format_edhoc_values_t expected_format,
    const uint8_t** payload, size_t* payload_len);

coap_status_result_t successful_add_response_options_stub(
    coap_pdu_t* response, content_format_edhoc_values_t content_format);

coap_status_result_t failed_add_response_options_stub(
    coap_pdu_t* response, content_format_edhoc_values_t content_format);

void* get_null_session_app_data_stub(const coap_session_t* session);

void* get_non_null_session_app_data_stub(const coap_session_t* session);

bool is_message_1_stub(const uint8_t* payload, size_t payload_len);

bool is_not_message_1_stub(const uint8_t* payload, size_t payload_len);

edhoc_server_message_1_result_t failed_handle_message_1_stub(
    const edhoc_server_message_1_request_data_t* request_data,
    common_response_buffer_t* response_data);

edhoc_server_message_1_result_t successful_handle_message_1_stub(
    const edhoc_server_message_1_request_data_t* request_data,
    common_response_buffer_t* response_data);

edhoc_server_message_1_result_t
successful_handle_message_1_with_valid_payload_length_stub(
    const edhoc_server_message_1_request_data_t* request_data,
    common_response_buffer_t* response_data);

coap_pdu_code_t successful_process_message_1_stub(
    edhoc_server_message_1_result_t message_1_result, coap_session_t* session);

coap_pdu_code_t failed_process_message_1_stub(
    edhoc_server_message_1_result_t message_1_result, coap_session_t* session);

bool is_message_3_stub(const uint8_t* request_payload, size_t request_len,
                       const struct edhoc_context* edhoc_ctx,
                       struct edhoc_extracted_fields* extracted_fields);

bool is_not_message_3_stub(const uint8_t* request_payload, size_t request_len,
                           const struct edhoc_context* edhoc_ctx,
                           struct edhoc_extracted_fields* extracted_fields);

coap_pdu_code_t successful_handle_message_3_stub(
    const edhoc_server_message_3_request_data_t* request_data,
    common_response_buffer_t* response_data);

coap_pdu_code_t failed_handle_message_3_stub(
    const edhoc_server_message_3_request_data_t* request_data,
    common_response_buffer_t* response_data);

coap_status_result_t successful_add_response_payload_stub(
    coap_pdu_t* response, const uint8_t* payload, size_t payload_len);

coap_status_result_t failed_add_response_payload_stub(coap_pdu_t* response,
                                                      const uint8_t* payload,
                                                      size_t payload_len);

#endif  // COAP_SERVER_DISPATCH_STUBS_H_