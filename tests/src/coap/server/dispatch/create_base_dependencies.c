#include "coap/server/dispatch/create_base_dependencies.h"

#include "coap/server/dispatch/stubs.h"

cp_serv_dispatch_deps_t test_cp_srv_dispatch_create_base_dependencies(void) {
  return (cp_serv_dispatch_deps_t){
      .extract_payload_if_valid_edhoc_request =
          stb_cp_srv_extract_payload_success,
      .add_edhoc_response_options = stb_cp_srv_add_options_success,
      .parse_message_1 = stb_edh_srv_parse_message_1_true,
      .parse_message_3 = stb_edh_srv_parse_message_3_false,
      .handle_message_1 = stb_edh_srv_handle_m1_ok,
      .process_message_1_result = stb_cp_srv_process_m1_ok,
      .handle_message_3 = stb_edh_srv_handle_m3_ok,
      .process_message_3_result = stb_cp_srv_process_m3_ok,
      .add_response_payload = stb_cp_srv_add_payload_ok,
      .get_session_app_data = stb_cp_srv_get_session_valid,
  };
}
