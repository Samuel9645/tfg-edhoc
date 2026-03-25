#include "coap/server/test_edhoc_dispatch_deps_builder.h"

coap_server_edhoc_dispatch_deps_t make_base_deps(void) {
  return (coap_server_edhoc_dispatch_deps_t){
      .extract_payload_if_valid_edhoc_request = successful_extract_payload_stub,
      .add_edhoc_response_options = successful_add_response_options_stub,
      .is_message_1 = is_message_1_stub,
      .is_message_3 = is_not_message_3_stub,
      .handle_message_1 = successful_handle_message_1_stub,
      .handle_message_3 = successful_handle_message_3_stub,
      .add_response_payload = successful_add_response_payload_stub,
      .get_session_app_data = get_non_null_session_app_data_stub,
  };
}
