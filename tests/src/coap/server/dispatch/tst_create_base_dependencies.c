#include "coap/server/dispatch/tst_create_base_dependencies.h"

#include "coap/server/dispatch/tst_srv_dispatch_stubs.h"

struct srv_coap_dispatch_deps test_srv_coap_dispatch_create_base_dependencies(
    void) {
  return (struct srv_coap_dispatch_deps){
      .parse_edhoc_request = stb_srv_coap_parse_edhoc_request_ok,
      .add_edhoc_response_options = stb_srv_coap_add_options_success,
      .parse_message_1 = stb_srv_edhoc_parse_m1_ok,
      .parse_message_3 = stb_srv_edhoc_parse_m3_protocol_failure,
      .handle_message_1 = stb_srv_edhoc_handle_m1_ok,
      .process_message_1_result = stb_srv_coap_process_m1_ok,
      .handle_message_3 = stb_srv_edhoc_handle_m3_ok,
      .process_message_3_result = stb_srv_coap_process_m3_ok,
      .add_response_payload = stb_srv_coap_add_payload_ok,
      .get_session_app_data = stb_srv_coap_get_session_valid,
  };
}
