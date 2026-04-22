#include "coap/server/srv_dispatch.h"

#include "coap/coap_config.h"
#include "coap/common/com_coap_parse_edhoc_request.h"
#include "coap/common/com_coap_response.h"
#include "coap/server/edhoc_message_process/srv_coap_m1_process.h"
#include "coap/server/edhoc_message_process/srv_coap_m3_process.h"
#include "coap/server/internal/srv_dispatch_engine.h"
#include "edhoc/server/handshake/message_3/srv_m3_responder.h"

static const struct srv_coap_dispatch_deps
    coap_server_edhoc_dispatch_default_deps = {
        .parse_edhoc_request = com_coap_parse_edhoc_request,
        .add_edhoc_response_options = com_coap_add_edhoc_response_options,
        .respond_to_message_1 = srv_edhoc_respond_to_message_1,
        .process_message_1_result = srv_coap_process_message_1_result,
        .respond_to_message_3 = srv_edhoc_respond_to_message_3,
        .process_message_3_result = srv_coap_process_message_3_result,
        .add_response_payload = com_coap_add_response_payload,
        .get_session_app_data = coap_session_get_app_data,
};

void srv_coap_dispatch_edhoc_post(coap_session_t* session,
                                  const coap_pdu_t* request,
                                  const struct edhoc_credentials* credentials,
                                  coap_pdu_t* response) {
  srv_coap_dispatch_post_with_dependencies(
      session, request, credentials, response,
      &coap_server_edhoc_dispatch_default_deps);
}
