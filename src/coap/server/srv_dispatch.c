#include "coap/server/srv_dispatch.h"

#include "coap/coap_config.h"
#include "coap/common/com_coap_helpers.h"
#include "coap/common/com_coap_response.h"
#include "coap/server/edhoc_mapper/srv_m1_mapper.h"
#include "coap/server/edhoc_mapper/srv_m3_mapper.h"
#include "coap/server/internal/srv_dispatch_engine.h"
#include "coap/server/srv_parse_edhoc_request.h"
#include "edhoc/server/handshake/message_1/srv_m1_parser.h"
#include "edhoc/server/handshake/message_3/srv_m3_handler.h"
#include "edhoc/server/handshake/message_3/srv_m3_parser.h"

// TODO: specific error reporting

static enum status_coap add_edhoc_response_options(
    coap_pdu_t* response,
    const enum config_coap_content_format_edhoc_values content_format) {
  if (response == NULL) {
    coap_log_err("response pdu is null\n");
    return STATUS_COAP_ERR;
  }

  coap_optlist_t* optlist = com_coap_create_coap_edhoc_optlist(content_format);
  if (optlist == NULL) {
    coap_log_err("cannot create options list\n");
    return STATUS_COAP_ERR;
  }

  if (!coap_add_optlist_pdu(response, &optlist)) {
    coap_log_err("cannot add options to response\n");
    coap_delete_optlist(optlist);
    return STATUS_COAP_ERR;
  }
  coap_delete_optlist(optlist);
  return STATUS_COAP_OK;
}

static const struct srv_coap_dispatch_deps
    coap_server_edhoc_dispatch_default_deps = {
        .parse_edhoc_request = srv_coap_parse_edhoc_request,
        .add_edhoc_response_options = add_edhoc_response_options,
        .parse_message_1 = srv_edhoc_parse_message_1,
        .parse_message_3 = srv_edhoc_parse_message_3,
        .handle_message_1 = srv_edhoc_handle_message_1,
        .process_message_1_result = srv_coap_map_message_1_result_to_coap,
        .handle_message_3 = srv_edhoc_handle_message_3,
        .process_message_3_result = srv_coap_map_message_3_result_to_coap,
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
