#include "coap/server/dispatch.h"

#include "coap/common/helpers.h"
#include "coap/common/response.h"
#include "coap/config.h"
#include "coap/server/edhoc_mapper/message_1_mapper.h"
#include "coap/server/edhoc_mapper/message_3_mapper.h"
#include "coap/server/request.h"
#include "edhoc/server/handshake/message_1/parser.h"
#include "edhoc/server/handshake/message_3/handler.h"
#include "edhoc/server/handshake/message_3/parser.h"
#include "internal/dispatch_engine.h"

static cp_status_t add_edhoc_response_options(
    coap_pdu_t* response,
    const cp_cfg_content_format_edhoc_values_t content_format) {
  if (!response) {
    coap_log_err("response pdu is null\n");
    return CP_STATUS_ERROR;
  }

  coap_optlist_t* optlist = cp_com_create_coap_edhoc_optlist(content_format);
  if (!optlist) {
    coap_log_err("cannot create options list\n");
    return CP_STATUS_ERROR;
  }

  if (!coap_add_optlist_pdu(response, &optlist)) {
    coap_log_err("cannot add options to response\n");
    coap_delete_optlist(optlist);
    return CP_STATUS_ERROR;
  }
  coap_delete_optlist(optlist);
  return CP_STATUS_SUCCESS;
}

static const cp_serv_dispatch_deps_t coap_server_edhoc_dispatch_default_deps = {
    .extract_payload_if_valid_edhoc_request =
        cp_srv_extract_payload_if_valid_edhoc_request,
    .add_edhoc_response_options = add_edhoc_response_options,
    .parse_message_1 = edh_srv_parse_message_1,
    .parse_message_3 = edh_srv_parse_message_3,
    .handle_message_1 = edh_srv_handle_message_1,
    .process_message_1_result = cp_srv_map_message_1_result_to_coap,
    .handle_message_3 = edh_srv_handle_message_3,
    .process_message_3_result = cp_srv_map_message_3_result_to_coap,
    .add_response_payload = cp_com_add_response_payload,
    .get_session_app_data = coap_session_get_app_data,
};

void cp_srv_dispatch_edhoc_post(coap_session_t* session,
                                const coap_pdu_t* request,
                                const struct edhoc_credentials* credentials,
                                coap_pdu_t* response) {
  cp_srv_dispatch_post_with_dependencies(
      session, request, credentials, response,
      &coap_server_edhoc_dispatch_default_deps);
}
