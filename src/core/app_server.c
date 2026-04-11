#include "app/app_server.h"

#include <coap3/coap.h>

#include "coap/common/com_coap_context.h"
#include "coap/server/srv_dispatch.h"
#include "coap/server/srv_utils.h"
#include "common/com_emulation.h"
#include "common/com_session_resources.h"
#include "edhoc/credentials/cred_srv.h"

// ReSharper disable once CppParameterMayBeConstPtrOrRef
static void edhoc_post_handler(coap_resource_t* resource,
                               coap_session_t* session,
                               const coap_pdu_t* request,
                               const coap_string_t* query,
                               coap_pdu_t* response) {
  (void)resource;
  (void)query;
  srv_coap_dispatch_edhoc_post(session, request, &CRED_EDHOC_SRV_CRED,
                               response);
}

enum com_emulation_status core_run_server(void) {
  coap_startup();
  const struct com_coap_create_context_result initialization_result =
      com_coap_create_context();
  if (initialization_result.status != COM_COAP_INIT_OK) {
    return COM_EMULATION_FAILURE;
  }
  struct com_session_resources server_resources = {
      .coap_context = initialization_result.context,
  };
  static const char COAP_LISTEN_UCAST_IP[] = "::";
  if (srv_coap_setup_endpoints(server_resources.coap_context,
                               COAP_LISTEN_UCAST_IP) != STATUS_COAP_OK) {
    com_cleanup_resources(&server_resources);
    return COM_EMULATION_FAILURE;
  }

  if (srv_coap_add_post_resource(server_resources.coap_context,
                                 ".well-known/edhoc",
                                 edhoc_post_handler) != STATUS_COAP_OK) {
    com_cleanup_resources(&server_resources);
    return COM_EMULATION_FAILURE;
  }

  if (srv_coap_run_input_output_loop(server_resources.coap_context) !=
      STATUS_COAP_OK) {
    com_cleanup_resources(&server_resources);
    return COM_EMULATION_FAILURE;
  }

  com_cleanup_resources(&server_resources);
  return COM_EMULATION_SUCCESS;
}
