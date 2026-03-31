#include "coap/server/dispatch.h"

#include <edhoc_helpers.h>

#include "coap/coap_config.h"
#include "coap/common/helpers.h"
#include "coap/common/response.h"
#include "coap/server/request.h"
#include "edhoc/edhoc_config.h"
#include "edhoc/server/handshake.h"
#include "internal/dispatch_engine.h"

static coap_status_result_t add_edhoc_response_options(
    coap_pdu_t* response, content_format_edhoc_values_t content_format) {
  if (!response) {
    coap_log_err("response pdu is null\n");
    return CCOM_ERROR;
  }

  coap_optlist_t* optlist =
      coap_common_create_coap_edhoc_optlist(content_format);
  if (!optlist) {
    coap_log_err("cannot create options list\n");
    return CCOM_ERROR;
  }

  if (!coap_add_optlist_pdu(response, &optlist)) {
    coap_log_err("cannot add options to response\n");
    coap_delete_optlist(optlist);
    return CCOM_ERROR;
  }
  coap_delete_optlist(optlist);
  return CCOM_STATUS_SUCCESS;
}

static const coap_server_dispatch_deps_t
    coap_server_edhoc_dispatch_default_deps = {
        .extract_payload_if_valid_edhoc_request =
            coap_server_extract_payload_if_valid_edhoc_request,
        .add_edhoc_response_options = add_edhoc_response_options,
        .is_message_1 = edhoc_server_is_properly_formatted_message_1,
        .is_message_3 = edhoc_server_extract_if_properly_formatted_message_3,
        .handle_message_1 = edhoc_server_handle_message_1,
        .handle_message_3 = edhoc_server_handle_message_3,
        .add_response_payload = coap_common_add_response_payload,
        .get_session_app_data = coap_session_get_app_data,
};

void coap_server_dispatch_edhoc_post(coap_session_t* session,
                                     const coap_pdu_t* request,
                                     coap_pdu_t* response) {
  coap_server_dispatch_post_with_dependencies(
      session, request, response, &coap_server_edhoc_dispatch_default_deps);
}
