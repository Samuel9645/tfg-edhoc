// ReSharper disable CppParameterMayBeConstPtrOrRef
#include "app/app_server.h"

#include <coap3/coap.h>
#include <string.h>

#include "coap/common/com_coap_context.h"
#include "coap/server/srv_dispatch.h"
#include "coap/server/srv_utils.h"
#include "common/com_emulation.h"
#include "common/com_session_resources.h"

// ReSharper disable once CppParameterMayBeConstPtrOrRef
static void edhoc_post_handler(coap_resource_t* resource,
                               coap_session_t* session,
                               const coap_pdu_t* request,
                               const coap_string_t* query,
                               coap_pdu_t* response) {
  (void)resource;
  (void)query;
  const struct com_edhoc_parameters* edhoc_parameters =
      coap_context_get_app_data(coap_session_get_context(session));
  srv_coap_dispatch_edhoc_post(coap_session_get_context(session), request,
                               *edhoc_parameters, response);
}

static void temperature_get_handler(coap_resource_t* resource,
                                    coap_session_t* session,
                                    const coap_pdu_t* request,
                                    const coap_string_t* query,
                                    coap_pdu_t* response) {
  (void)resource;
  (void)query;
  (void)session;
  (void)request;
  const char* temp_data = "{\"temp\": 23.5, \"unit\": \"C\"}";
  coap_add_data(response, strlen(temp_data), (const uint8_t*)temp_data);
  coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
}

enum com_emulation_status core_run_server(
    struct com_edhoc_parameters* edhoc_parameters) {
  coap_startup();
  coap_set_log_level(COAP_LOG_DEBUG);
  const struct com_coap_create_context_result initialization_result =
      com_coap_create_context();
  if (initialization_result.status != COM_COAP_INIT_OK) {
    return COM_EMULATION_FAILURE;
  }
  if (coap_context_set_app_data2(initialization_result.context,
                                 edhoc_parameters, NULL) != NULL) {
    coap_log_err("THIS SHOULD NEVER HAPPEN: FATAL, CONTEXT ALREADY HAVE DATA");
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
  if (srv_coap_add_oscore_only_get_resource(
          server_resources.coap_context, "sensors/temperature",
          temperature_get_handler) != STATUS_COAP_OK) {
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
