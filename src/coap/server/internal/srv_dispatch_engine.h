#ifndef COAP_SERVER_INTERNAL_dispatch_engine_H_
#define COAP_SERVER_INTERNAL_dispatch_engine_H_

#include <coap3/coap.h>

#include "coap/coap_config.h"
#include "coap/common/com_coap_status.h"
#include "coap/server/srv_parse_edhoc_request.h"
#include "common/com_data_models.h"
#include "edhoc/server/handshake/message_1/srv_m1_handler.h"
#include "edhoc/server/handshake/message_1/srv_m1_parser.h"
#include "edhoc/server/handshake/message_3/srv_m3_handler.h"
#include "edhoc/server/handshake/message_3/srv_m3_parser.h"

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
  struct srv_coap_parse_edhoc_request_result (*parse_edhoc_request)(
      const coap_pdu_t* request,
      enum config_coap_content_format_edhoc_values expected_format,
      struct com_writable_buffer* data_buffer);

  /** Adds EDHOC-specific content-format option to outgoing CoAP response. */
  enum status_coap (*add_edhoc_response_options)(
      coap_pdu_t* response,
      enum config_coap_content_format_edhoc_values content_format);

  /** Parses EDHOC Message 1 and returns the stripped payload view. */
  struct srv_edhoc_parse_message_1_result (*parse_message_1)(
      struct com_readonly_buffer readonly_buffer);

  /**
   * Parses EDHOC Message 3 payload (with prepended connection ID) and extracts
   * Message 3 fields for handler processing.
   */
  struct srv_edhoc_parse_message_3_result (*parse_message_3)(
      struct com_readonly_buffer request_buffer,
      const struct edhoc_context* edhoc_ctx);

  /** Processes EDHOC Message 1 and generates Message 2 response. */
  struct srv_edhoc_message_1_handler_result (*handle_message_1)(
      struct srv_edhoc_message_1_request request_data,
      struct com_writable_buffer* response_data);
  /** Processes the result of EDHOC Message 1 handling, linking the EDHOC
   * logic with the CoAP transport layer and returning the response code. */
  coap_pdu_code_t (*process_message_1_result)(
      struct srv_edhoc_message_1_handler_result message_1_result,
      coap_session_t* session);

  /** Processes EDHOC Message 3 and generates Message 4 response. */
  enum srv_edhoc_message_3_handler_status (*handle_message_3)(
      struct srv_edhoc_message_3_request request_data,
      struct com_writable_buffer* response_data);
  /** Processes the result of EDHOC Message 3 handling and returns the CoAP
   * response code. */
  coap_pdu_code_t (*process_message_3_result)(
      enum srv_edhoc_message_3_handler_status message_3_result);

  /** Adds response payload bytes to outgoing CoAP PDU. */
  enum status_coap (*add_response_payload)(coap_pdu_t* response,
                                           const uint8_t* payload,
                                           size_t payload_len);

  /** Retrieves application context data associated with a CoAP session. */
  void* (*get_session_app_data)(const coap_session_t* session);
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
void srv_coap_dispatch_post_with_dependencies(
    coap_session_t* session, const coap_pdu_t* request,
    const struct edhoc_credentials* credentials, coap_pdu_t* response,
    const struct srv_coap_dispatch_deps* deps);

#endif  // COAP_SERVER_INTERNAL_dispatch_engine_H_
