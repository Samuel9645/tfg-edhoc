#ifndef COAP_SERVER_INTERNAL_dispatch_engine_H_
#define COAP_SERVER_INTERNAL_dispatch_engine_H_

#include <coap3/coap.h>

#include "coap/coap_config.h"
#include "coap/common/com_coap_parse_edhoc_request.h"
#include "coap/common/com_coap_status.h"
#include "coap/server/extract_edhoc_message/srv_coap_extract_m3.h"
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

typedef struct srv_coap_extract_message_1_result (*srv_coap_parse_message_1_fn)(
    struct com_readonly_buffer request_buffer,
    struct com_writable_buffer error_response);

typedef struct srv_coap_extract_connection_id_result (*srv_coap_extract_cid_fn)(
    struct com_readonly_buffer request_buffer);

typedef bool (*srv_coap_connection_id_is_expected_fn)(
    const struct edhoc_connection_id* extracted_cid,
    const struct edhoc_context* edhoc_ctx);

typedef struct srv_edhoc_message_1_responder_result (
    *srv_edhoc_m1_responder_fn)(
    struct srv_edhoc_message_1_responder_request request_data,
    struct com_edhoc_parameters edhoc_parameters,
    struct com_writable_buffer response_data);

typedef coap_pdu_code_t (*srv_edhoc_m1_process_result_fn)(
    struct srv_edhoc_message_1_responder_result message_1_result,
    coap_session_t* session);

typedef struct srv_edhoc_message_3_responder_result (
    *srv_edhoc_m3_responder_fn)(
    struct srv_edhoc_message_3_responder_request request_data,
    struct com_writable_buffer response_data);

typedef coap_pdu_code_t (*srv_edhoc_m3_process_result_fn)(
    struct srv_edhoc_message_3_responder_result message_3_result);

typedef enum status_coap (*srv_coap_add_response_payload_fn)(
    coap_pdu_t* response, const uint8_t* payload, size_t payload_len);

typedef void* (*srv_coap_get_session_app_data_fn)(
    const coap_session_t* session);

/**
 * @brief Dependency injection structure for EDHOC dispatcher seam testing.
 *
 * Defines function pointers for all dispatch operations, allowing tests to
 * inject custom implementations and verify dispatcher behavior in isolation.
 * This enables unit testing of the dispatcher logic independent of actual
 * EDHOC processing and CoAP PDU handling.
 *
 * @note All pointers must be non-NULL when passed to the dispatcher.
 *
 * @see coap_server_dispatch_post_with_dependencies for usage.
 */
struct srv_coap_dispatch_deps {
  /** Validates incoming CoAP PDU and extracts EDHOC message payload. */
  srv_coap_parse_edhoc_request_fn parse_edhoc_request;

  /** Adds EDHOC-specific content-format option to outgoing CoAP response. */
  srv_coap_add_edhoc_response_options_fn add_edhoc_response_options;

  /** Parses Message 1, removing the CBOR prefix */
  srv_coap_parse_message_1_fn extract_message_1;

  /** Extracts the connection ID and the remaining Message 3 payload. */
  srv_coap_extract_cid_fn extract_cid;

  /** Validates that the extracted connection ID matches the EDHOC context. */
  srv_coap_connection_id_is_expected_fn connection_id_is_expected;

  /** Processes EDHOC Message 1 and generates Message 2 response. */
  srv_edhoc_m1_responder_fn respond_to_message_1;

  /** Processes the result of EDHOC Message 1 handling, linking the EDHOC
   * logic with the CoAP transport layer and returning the response code. */
  srv_edhoc_m1_process_result_fn process_message_1_result;

  /** Processes EDHOC Message 3 and generates Message 4 response. */
  srv_edhoc_m3_responder_fn respond_to_message_3;

  /** Processes the result of EDHOC Message 3 handling and returns the CoAP
   * response code. */
  srv_edhoc_m3_process_result_fn process_message_3_result;

  /** Adds response payload bytes to outgoing CoAP PDU. */
  srv_coap_add_response_payload_fn add_response_payload;

  /** Retrieves application context data associated with a CoAP session. */
  srv_coap_get_session_app_data_fn get_session_app_data;
};

/**
 * @brief Dispatch incoming EDHOC-over-CoAP POST request with injected
 * dependencies.
 *
 * Core seam function for testing and production. Routes incoming requests to
 * Message 1 or Message 3 handlers based on payload format and session state.
 * Validates requests, adds response options, handles errors, and propagates
 * response codes according to RFC 9528 A.2 behavior.
 *
 * @param[in] session Active CoAP session bound to the remote client.
 * @param[in] request Incoming CoAP PDU with EDHOC message payload.
 * @param[in] edhoc_parameters EDHOC credentials for the session.
 * @param[in,out] response CoAP response PDU where code and payload are set.

 * @param[in] deps Dependency injection structure with custom implementations
 * for all dispatch operations. All function pointers must be non-NULL.
 *
 * @note On validation or early error paths, response code is set but payload
 * may remain empty. Callers must check both response code and payload length.
 *
 * @warning This is an internal seam function for unit testing. Production
 * code should call coap_server_dispatch_edhoc_post() from the public API
 * instead, which provides default implementations of all dependencies.
 *
 * @see [RFC 9528 A.2 - EDHOC Message
 * Flow](https://datatracker.ietf.org/doc/html/rfc9528/#name-the-forward-message-flow)
 * for protocol details on Message 1 and Message 3 handling.
 */
void srv_coap_dispatch_post_with_dependencies(
    coap_session_t* session, const coap_pdu_t* request,
    struct com_edhoc_parameters edhoc_parameters, coap_pdu_t* response,
    const struct srv_coap_dispatch_deps* deps);

#endif  // COAP_SERVER_INTERNAL_dispatch_engine_H_
