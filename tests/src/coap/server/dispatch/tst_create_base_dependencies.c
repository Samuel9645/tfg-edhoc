#include "coap/server/dispatch/tst_create_base_dependencies.h"

#include "coap/server/dispatch/tst_srv_dispatch_stubs.h"

struct cp_srv_dispatch_deps test_cp_srv_dispatch_create_base_dependencies(
    void) {
  return (struct cp_srv_dispatch_deps){
      .parse_edhoc_request = stb_cp_srv_parse_edhoc_request_ok,
      .add_edhoc_response_options = stb_cp_srv_add_options_success,
      .parse_message_1 = stb_edh_srv_parse_message_1_ok,
      .parse_message_3 = stb_edh_srv_parse_message_3_failure,
      .handle_message_1 = stb_edh_srv_handle_m1_ok,
      .process_message_1_result = stb_cp_srv_process_m1_ok,
      .handle_message_3 = stb_edh_srv_handle_m3_ok,
      .process_message_3_result = stb_cp_srv_process_m3_ok,
      .add_response_payload = stb_cp_srv_add_payload_ok,
      .get_session_app_data = stb_cp_srv_get_session_valid,
  };
}
