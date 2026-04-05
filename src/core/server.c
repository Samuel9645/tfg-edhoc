#include "core/server.h"

#include <coap3/coap.h>

#include "coap/server/dispatch.h"
#include "coap/server/utils.h"
#include "common/cleanup.h"
#include "edhoc/credentials/server_credentials.h"

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
  struct com_session_resources server_resources = {0};

  server_resources.coap_context = cp_srv_create_context();
  if (!server_resources.coap_context) {
    com_cleanup_resources(&server_resources);
    return COM_EMULATION_FAILURE;
  }

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
