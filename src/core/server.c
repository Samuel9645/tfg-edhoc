#include "core/server.h"

#include <coap3/coap.h>

#include "coap/server/dispatch.h"
#include "coap/server/utils.h"
#include "common/cleanup.h"
#include "edhoc/credentials/server_credentials.h"

static void edhoc_post_handler(coap_resource_t* resource,
                               coap_session_t* session,
                               const coap_pdu_t* request,
                               const coap_string_t* query,
                               coap_pdu_t* response) {
  (void)resource;
  (void)query;
  coap_server_dispatch_edhoc_post(session, request, &SERVER_CREDENTIALS,
                                  response);
}

emulation_status_t tfg_run_server(void) {
  session_resources_t server_resources = {0};

  server_resources.coap_context = coap_server_create_context();
  if (!server_resources.coap_context) {
    tfg_common_cleanup_resources(&server_resources);
    return EMULATION_FAILURE;
  }

  static const char COAP_LISTEN_UCAST_IP[] = "::";
  if (coap_server_setup_endpoints(server_resources.coap_context,
                                  COAP_LISTEN_UCAST_IP) !=
      CCOM_STATUS_SUCCESS) {
    tfg_common_cleanup_resources(&server_resources);
    return EMULATION_FAILURE;
  }

  if (coap_server_add_post_resource(server_resources.coap_context,
                                    ".well-known/edhoc", edhoc_post_handler) !=
      CCOM_STATUS_SUCCESS) {
    tfg_common_cleanup_resources(&server_resources);
    return EMULATION_FAILURE;
  }

  if (coap_server_run_input_output_loop(server_resources.coap_context) !=
      CCOM_STATUS_SUCCESS) {
    tfg_common_cleanup_resources(&server_resources);
    return EMULATION_FAILURE;
  }

  tfg_common_cleanup_resources(&server_resources);
  return EMULATION_SUCCESS;
}
