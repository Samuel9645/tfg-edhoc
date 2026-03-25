/**
 * @file test_edhoc_dispatch_stubs.h
 *
 * @brief Stub implementations used by EDHOC dispatcher seam unit tests.
 *
 * Each function updates trace state and returns deterministic results to drive
 * success and error paths through the dispatcher.
 */
#ifndef TEST_EDHOC_DISPATCH_STUBS_H_
#define TEST_EDHOC_DISPATCH_STUBS_H_

#include "coap/server/test_edhoc_dispatch_fixture.h"

/**
 * @brief Returns the configured fake payload.
 */
coap_status_result_t extract_fake_payload(
    const coap_pdu_t* request, content_format_edhoc_values_t expected_format,
    const uint8_t** payload, size_t* payload_len);

/**
 * @brief Returns an error status without setting payload output parameters.
 */
coap_status_result_t fail_to_extract_payload(
    const coap_pdu_t* request, content_format_edhoc_values_t expected_format,
    const uint8_t** payload, size_t* payload_len);

/**
 * @brief Response option adder that forces an error status.
 */
coap_status_result_t add_edhoc_response_options_fail(
    coap_pdu_t* response, content_format_edhoc_values_t content_format);

/**
 * @brief Response option adder that forces a success status.
 */
coap_status_result_t add_edhoc_response_options_ok(
    coap_pdu_t* response, content_format_edhoc_values_t content_format);

/**
 * @brief Message 1 checker stub that always rejects the payload.
 */
bool is_not_message_1_format(const uint8_t* payload, size_t payload_len);

/**
 * @brief Message 3 checker stub that always rejects the payload.
 */
bool is_not_message_3_format(const uint8_t* request_payload, size_t request_len,
                             const struct edhoc_context* edhoc_ctx,
                             struct edhoc_extracted_fields* extracted_fields);

/**
 * @brief Message 1 format checker using a simple test predicate.
 */
bool is_message_1_format(const uint8_t* payload, size_t payload_len);

/**
 * @brief Message 3 format checker validating prepended C_R and context
 * presence.
 */
bool is_message_3_format(const uint8_t* request_payload, size_t request_len,
                         const struct edhoc_context* edhoc_ctx,
                         struct edhoc_extracted_fields* extracted_fields);

/**
 * @brief Message 1 handler stub that reports internal error with empty payload.
 */
coap_pdu_code_t handle_message_1_internal_error_with_empty_payload(
    const server_edhoc_message_1_request_data_t* request_data,
    coap_response_data_t* response_data);

/**
 * @brief Message 1 handler stub that reports success with one-byte payload.
 */
coap_pdu_code_t handle_message_1_changed_with_payload(
    const server_edhoc_message_1_request_data_t* request_data,
    coap_response_data_t* response_data);

/**
 * @brief Message 1 handler stub that reports internal error with error payload.
 */
coap_pdu_code_t handle_message_1_internal_error_with_payload(
    const server_edhoc_message_1_request_data_t* request_data,
    coap_response_data_t* response_data);

/**
 * @brief Message 3 handler stub that reports success with empty payload.
 */
coap_pdu_code_t handle_message_3_changed(
    const server_edhoc_message_3_request_data_t* request_data,
    coap_response_data_t* response_data);

/**
 * @brief Message 3 handler stub that prepends C_R and reports success.
 */
coap_pdu_code_t handle_message_3_changed_with_prepended_c_r(
    const server_edhoc_message_3_request_data_t* request_data,
    coap_response_data_t* response_data);

/**
 * @brief Message 3 handler stub that prepends C_R and reports bad request.
 */
coap_pdu_code_t handle_message_3_bad_request_with_error_payload(
    const server_edhoc_message_3_request_data_t* request_data,
    coap_response_data_t* response_data);

/**
 * @brief Shared helper that records payload bytes and returns selected status.
 */
coap_status_result_t add_response_payload_with_status(coap_pdu_t* response,
                                                      const uint8_t* payload,
                                                      size_t payload_len,
                                                      bool status_to_return);

/**
 * @brief Payload adder stub that forces success.
 */
coap_status_result_t add_response_payload_ok(coap_pdu_t* response,
                                             const uint8_t* payload,
                                             size_t payload_len);

/**
 * @brief Payload adder stub that forces failure.
 */
coap_status_result_t add_response_payload_fail(coap_pdu_t* response,
                                               const uint8_t* payload,
                                               size_t payload_len);

/**
 * @brief Returns app-data from fake session.
 */
void* get_session_app_data(const coap_session_t* session);

/**
 * @brief Captures response code updates in fake response model.
 */
void set_response_code(coap_pdu_t* response, coap_pdu_code_t code);

/**
 * @brief No-op logger used to silence expected error-path logs in tests.
 */
void log_error_noop(const char* message);

#endif  // TEST_EDHOC_DISPATCH_STUBS_H_
