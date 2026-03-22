#ifndef COAP_SERVER_INTERNAL_EDHOC_DISPATCH_INTERNAL_H_
#define COAP_SERVER_INTERNAL_EDHOC_DISPATCH_INTERNAL_H_

#include <coap3/coap.h>
#include <edhoc_helpers.h>

#include "coap/common/config.h"
#include "coap/common/data_models.h"
#include "coap/common/status.h"
#include "coap/server/edhoc_dispatch.h"
#include "edhoc/server/server_flow.h"

typedef struct coap_server_edhoc_dispatch_deps_t {
  coap_status_result_t (*validate_edhoc_request)(
      const coap_pdu_t* request, content_format_edhoc_values_t expected_format,
      const uint8_t** payload, size_t* payload_len);
  coap_status_result_t (*add_edhoc_response_options)(
      coap_pdu_t* response, content_format_edhoc_values_t content_format);
  bool (*is_message_1)(const uint8_t* payload, size_t payload_len);
  bool (*is_message_3)(const uint8_t* request_payload, size_t request_len,
                       const struct edhoc_context* edhoc_ctx,
                       struct edhoc_extracted_fields* extracted_fields);
  coap_pdu_code_t (*handle_message_1)(
      const server_edhoc_message_1_request_data_t* request_data,
      coap_response_data_t* response_data);
  coap_pdu_code_t (*handle_message_3)(
      const server_edhoc_message_3_request_data_t* request_data,
      coap_response_data_t* response_data);
  coap_status_result_t (*add_response_payload)(coap_pdu_t* response,
                                               const uint8_t* payload,
                                               size_t payload_len);
} coap_server_edhoc_dispatch_deps_t;

extern const coap_server_edhoc_dispatch_deps_t
    COAP_SERVER_EDHOC_DISPATCH_DEFAULT_DEPS;

void coap_server_dispatch_edhoc_post_with_deps(
    coap_session_t* session, const coap_pdu_t* request, coap_pdu_t* response,
    const coap_server_edhoc_dispatch_deps_t* deps);

#endif  // COAP_SERVER_INTERNAL_EDHOC_DISPATCH_INTERNAL_H_
