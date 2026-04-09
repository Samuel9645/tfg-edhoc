#include "core/app_server.h"

#include <coap3/coap.h>

#include "coap/common/coap_context.h"
#include "coap/server/srv_dispatch.h"
#include "coap/server/srv_utils.h"
#include "common/sys_cleanup.h"
#include "common/sys_emulation.h"
#include "edhoc/credentials/cred_srv.h"

// ReSharper disable once CppParameterMayBeConstPtrOrRef
static void edhoc_post_handler(coap_resource_t* resource,
                               coap_session_t* session,
                               const coap_pdu_t* request,
                               const coap_string_t* query,
                               coap_pdu_t* response) {
  (void)resource;
  (void)query;
  cp_srv_dispatch_edhoc_post(session, request, &EDH_CRED_SRV_CRED, response);
}

enum com_emulation_status core_run_server(void) {
  coap_startup();
  const struct cp_com_create_context_result initialization_result =
      cp_com_create_context();
  if (initialization_result.status != CP_COM_INIT_OK) {
    return COM_EMULATION_FAILURE;
  }
  struct com_session_resources server_resources = {
      .coap_context = initialization_result.context,
  };
  static const char COAP_LISTEN_UCAST_IP[] = "::";
  if (cp_srv_setup_endpoints(server_resources.coap_context,
                             COAP_LISTEN_UCAST_IP) != CP_STATUS_SUCCESS) {
    com_cleanup_resources(&server_resources);
    return COM_EMULATION_FAILURE;
  }

  if (cp_srv_add_post_resource(server_resources.coap_context,
                               ".well-known/edhoc",
                               edhoc_post_handler) != CP_STATUS_SUCCESS) {
    com_cleanup_resources(&server_resources);
    return COM_EMULATION_FAILURE;
  }

  if (cp_srv_run_input_output_loop(server_resources.coap_context) !=
      CP_STATUS_SUCCESS) {
    com_cleanup_resources(&server_resources);
    return COM_EMULATION_FAILURE;
  }

  com_cleanup_resources(&server_resources);
  return COM_EMULATION_SUCCESS;
}
