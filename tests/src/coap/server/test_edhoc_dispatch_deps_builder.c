#include "coap/server/test_edhoc_dispatch_deps_builder.h"

coap_server_edhoc_dispatch_deps_t make_base_deps(void) {
  return (coap_server_edhoc_dispatch_deps_t){
      .extract_payload_if_valid_edhoc_request = extract_fake_payload,
      .add_edhoc_response_options = add_edhoc_response_options_ok,
      .is_message_1 = is_message_1_format,
      .is_message_3 = is_message_3_format,
      .handle_message_1 = handle_message_1_internal_error_with_empty_payload,
      .handle_message_3 = handle_message_3_changed,
      .add_response_payload = add_response_payload_ok,
      .get_session_app_data = get_session_app_data,
      .set_response_code = set_response_code,
      .log_error = log_error_noop,
  };
}
