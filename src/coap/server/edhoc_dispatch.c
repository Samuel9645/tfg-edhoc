#include "coap/server/edhoc_dispatch.h"

#include <edhoc_helpers.h>

#include "coap/common/config.h"
#include "coap/server/internal/edhoc_dispatch_engine.h"
#include "coap/shared/edhoc_request.h"
#include "coap/shared/edhoc_response.h"
#include "edhoc/common/config.h"
#include "edhoc/server/flow.h"

static const coap_server_edhoc_dispatch_deps_t
    coap_server_edhoc_dispatch_default_deps = {
        .extract_payload_if_valid_edhoc_request =
            coap_shared_extract_payload_if_valid_edhoc_request,
        .add_edhoc_response_options = coap_shared_add_edhoc_response_options,
        .is_message_1 = coap_shared_is_properly_formatted_message_1,
        .is_message_3 = coap_shared_is_properly_formatted_message_3,
        .handle_message_1 = server_edhoc_handle_message_1,
        .handle_message_3 = server_edhoc_handle_message_3,
        .add_response_payload = coap_shared_add_response_payload,
        .get_session_app_data = coap_session_get_app_data,
};

void coap_server_dispatch_edhoc_post(coap_session_t* session,
                                     const coap_pdu_t* request,
                                     coap_pdu_t* response) {
  coap_server_dispatch_edhoc_post_with_dependencies(
      session, request, response, &coap_server_edhoc_dispatch_default_deps);
}
