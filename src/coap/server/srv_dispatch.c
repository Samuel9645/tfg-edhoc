#include "coap/server/srv_dispatch.h"

#include "coap/common/com_coap_parse_edhoc_request.h"
#include "coap/common/com_coap_pdu_helpers.h"
#include "coap/server/edhoc_message_process/srv_coap_m1_process.h"
#include "coap/server/edhoc_message_process/srv_coap_m3_process.h"
#include "coap/server/extract_edhoc_message/srv_coap_extract_m1.h"
#include "coap/server/extract_edhoc_message/srv_coap_extract_m3.h"
#include "coap/server/internal/srv_dispatch_engine.h"
#include "coap/server/internal/srv_session.h"
#include "coap/server/oscore/srv_oscore_bind_session.h"
#include "edhoc/server/handshake/message_3/srv_m3_responder.h"

static const struct srv_coap_dispatch_deps
    COAP_SERVER_EDHOC_DISPATCH_DEFAULT_DEPS = {
        .parse_edhoc_request = com_coap_parse_edhoc_request,
        .add_edhoc_response_options = com_coap_add_edhoc_options_to_pdu,
        .is_message_1 = srv_edhoc_is_message_1,
        .extract_message_1 = srv_coap_extract_message_1,
        .extract_cid = srv_coap_extract_connection_id,
        .connection_id_is_expected = srv_coap_connection_id_is_expected,
        .respond_to_message_1 = srv_edhoc_respond_to_message_1,
        .process_message_1_result = srv_coap_process_message_1_result,
        .respond_to_message_3 = srv_edhoc_respond_to_message_3,
        .process_message_3_result = srv_coap_process_message_3_result,
        .add_response_payload = com_coap_add_payload_to_pdu,
        .set_context_by_cid = srv_session_set_context_by_cid,
        .get_context_by_cid = srv_session_get_context_by_cid,
        .remove_context_by_cid = srv_session_remove_context_by_cid,
        .bind_oscore_session = srv_oscore_bind_session};

void srv_coap_dispatch_edhoc_post(
    const coap_pdu_t* request,
    const struct com_edhoc_parameters edhoc_parameters, coap_pdu_t* response,
    coap_context_t* context) {
  srv_coap_dispatch_post_with_dependencies(
      request, edhoc_parameters, response,
      &COAP_SERVER_EDHOC_DISPATCH_DEFAULT_DEPS, context);
}
