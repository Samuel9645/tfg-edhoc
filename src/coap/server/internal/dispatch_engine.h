#ifndef COAP_SERVER_INTERNAL_dispatch_engine_H_
#define COAP_SERVER_INTERNAL_dispatch_engine_H_

#include <coap3/coap.h>
#include <edhoc_helpers.h>

#include "coap/coap_config.h"
#include "coap/common/status.h"
#include "edhoc/server/handshake/message_1_handler.h"
#include "edhoc/server/handshake/message_3_handler.h"

/**
 * @brief Dependency injection structure for EDHOC dispatcher seam testing.
 *
 * Defines function pointers for all dispatch operations, allowing tests to
 * inject custom implementations and verify dispatcher behavior in isolation.
 * This enables unit testing of the dispatcher logic independent of actual
 * EDHOC processing and CoAP PDU handling.
 *
 * @note Each function pointer may be NULL if not used by a particular test
 * scenario, but all pointers must be non-NULL when passed to the dispatcher.
 *
 * @see coap_server_dispatch_post_with_dependencies for usage.
 */
typedef struct coap_server_dispatch_deps_t {
  /** Validates incoming CoAP PDU and extracts EDHOC message payload. */
  coap_status_result_t (*extract_payload_if_valid_edhoc_request)(
      const coap_pdu_t* request, content_format_edhoc_values_t expected_format,
      const uint8_t** payload, size_t* payload_len);

  /** Adds EDHOC-specific content-format option to outgoing CoAP response. */
  coap_status_result_t (*add_edhoc_response_options)(
      coap_pdu_t* response, content_format_edhoc_values_t content_format);

  /** Checks if payload conforms to EDHOC Message 1 format. */
  bool (*is_message_1)(const uint8_t* payload, size_t payload_len);

  /**
   * Checks if payload conforms to EDHOC Message 3 format (with prepended
   * connection ID). Also extracts Message 3 fields for handler processing.
   */
  bool (*extract_fields_if_message_3)(
      const uint8_t* request_payload, size_t request_len,
      const struct edhoc_context* edhoc_ctx,
      struct edhoc_extracted_fields* extracted_fields);

  /** Processes EDHOC Message 1 and generates Message 2 response. */
  edhoc_server_message_1_result_t (*handle_message_1)(
      const edhoc_server_message_1_request_data_t* request_data,
      common_response_buffer_t* response_data);
  /** Processes the result of EDHOC Message 1 handling, linking the EDHOC
   * logic with the CoAP transport layer and returning the response code. */
  coap_pdu_code_t (*process_message_1_result)(
      edhoc_server_message_1_result_t message_1_result,
      coap_session_t* session);

  /** Processes EDHOC Message 3 and generates Message 4 response. */
  edhoc_server_message_3_result_t (*handle_message_3)(
      const edhoc_server_message_3_request_data_t* request_data,
      common_response_buffer_t* response_data);
  /** Processes the result of EDHOC Message 3 handling and returns the CoAP
   * response code. */
  coap_pdu_code_t (*process_message_3_result)(
      edhoc_server_message_3_result_t message_3_result,
      coap_session_t* session);

  /** Adds response payload bytes to outgoing CoAP PDU. */
  coap_status_result_t (*add_response_payload)(coap_pdu_t* response,
                                               const uint8_t* payload,
                                               size_t payload_len);

  /** Retrieves application context data associated with a CoAP session. */
  void* (*get_session_app_data)(const coap_session_t* session);
} coap_server_dispatch_deps_t;

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
 * @param[in] credentials EDHOC credentials for the session.
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
void coap_server_dispatch_post_with_dependencies(
    coap_session_t* session, const coap_pdu_t* request,
    const struct edhoc_credentials* credentials, coap_pdu_t* response,
    const coap_server_dispatch_deps_t* deps);

#endif  // COAP_SERVER_INTERNAL_dispatch_engine_H_
