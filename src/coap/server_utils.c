#include "coap/server_utils.h"

#include <stdbool.h>

#include "coap/common/config.h"
#include "coap/common/helpers.h"
#include "coap/common/status.h"

coap_context_t* coap_server_create_context(void) {
  coap_startup();
  coap_set_log_level(COAP_LOG_WARN);

  coap_context_t* coap_context = coap_new_context(NULL);
  if (!coap_context) {
    coap_log_err("cannot create libcoap context\n");
    return NULL;
  }

  coap_context_set_block_mode(
      coap_context, COAP_SHARED_USE_LIBCOAP_FOR_REQUEST_AND_SINGLE_BODY_DATA);
  return coap_context;
}

coap_status_result_t coap_server_setup_endpoints(
    coap_context_t* coap_context, const char* listen_address_string) {
  if (!coap_context || !listen_address_string) {
    return COAP_STATUS_ERROR;
  }

  const bool has_pki_psk_info = false;
  const bool use_web_sockets = false;
  const uint32_t scheme_hints_bits = coap_get_available_scheme_hint_bits(
      has_pki_psk_info, use_web_sockets, COAP_PROTO_NONE);

  const coap_str_const_t* listen_address =
      coap_make_str_const(listen_address_string);
  enum { USE_DEFAULT_PORT_DATA = 0, NO_AI_HINT_FLAGS = 0 };
  coap_addr_info_t* endpoint_info_list = coap_resolve_address_info(
      listen_address, USE_DEFAULT_PORT_DATA, USE_DEFAULT_PORT_DATA,
      USE_DEFAULT_PORT_DATA, USE_DEFAULT_PORT_DATA, NO_AI_HINT_FLAGS,
      scheme_hints_bits, COAP_RESOLVE_TYPE_LOCAL);

  bool has_endpoint = false;
  for (coap_addr_info_t* endpoint_info = endpoint_info_list;
       endpoint_info != NULL; endpoint_info = endpoint_info->next) {
    coap_endpoint_t* endpoint = coap_new_endpoint(
        coap_context, &endpoint_info->addr, endpoint_info->proto);
    if (!endpoint) {
      coap_log_warn("cannot create endpoint for CoAP proto %u\n",
                    endpoint_info->proto);
    } else {
      has_endpoint = true;
    }
  }

  coap_free_address_info(endpoint_info_list);
  if (!has_endpoint) {
    coap_log_err("No context available for interface '%s'\n",
                 (const char*)listen_address->s);
    return COAP_STATUS_ERROR;
  }

  return COAP_STATUS_SUCCESS;
}

coap_status_result_t coap_server_join_multicast_group(
    coap_context_t* coap_context, const char* multicast_address_string) {
  if (!coap_context || !multicast_address_string) {
    return COAP_STATUS_ERROR;
  }

  const int join_result =
      coap_join_mcast_group_intf(coap_context, multicast_address_string, NULL);
  if (join_result < 0) {
    coap_log_warn("cannot join multicast group %s\n", multicast_address_string);
    return COAP_STATUS_ERROR;
  }

  return COAP_STATUS_SUCCESS;
}

coap_status_result_t coap_server_add_post_resource(
    coap_context_t* coap_context, const char* resource_path,
    coap_method_handler_t resource_handler) {
  if (!coap_context || !resource_path || !resource_handler) {
    return COAP_STATUS_ERROR;
  }

  enum { MEMORY_HANDLING_FLAGS = 0 };
  coap_resource_t* resource = coap_resource_init(
      coap_make_str_const(resource_path), MEMORY_HANDLING_FLAGS);
  if (!resource) {
    coap_log_err("cannot create resource\n");
    return COAP_STATUS_ERROR;
  }

  coap_register_request_handler(resource, COAP_REQUEST_POST, resource_handler);
  coap_add_resource(coap_context, resource);
  return COAP_STATUS_SUCCESS;
}

coap_status_result_t coap_server_run_input_output_loop(
    coap_context_t* coap_context) {
  if (!coap_context) {
    return COAP_STATUS_ERROR;
  }

  while (true) {
    if (coap_io_process(coap_context, COAP_IO_WAIT) < 0) {
      coap_log_err("CoAP I/O process failed\n");
      return COAP_STATUS_ERROR;
    }
  }
}
