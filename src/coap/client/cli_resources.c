// ReSharper disable CppClassNeverUsed
#include "coap/client/cli_resources.h"

#include <stdlib.h>
#include <string.h>

#include "common/com_session_resources.h"
#include "edhoc/common/com_edhoc_context_setup.h"

/**
 * @brief Infrastructure sub-structure: CoAP context, session, and endpoint
 * information. These resources are persistent across EDHOC session resets.
 */
struct cli_coap_infrastructure_resources {
  struct com_session_resources common_resources;
  coap_session_t* coap_session;
  coap_uri_t uri;
  coap_address_t destination;
};

/**
 * @brief Session sub-structure: EDHOC context and exchange handler. These
 * resources can be reset independently to attempt new handshakes without
 * reparsin URIs or re-resolving DNS.
 */
struct cli_coap_session_data {
  struct edhoc_context edhoc_ctx;
  struct cli_coap_exchange* exchange;
};

struct cli_resources {
  struct cli_coap_infrastructure_resources infrastructure;
  struct cli_coap_session_data session;
  uint8_t payload_memory[CONFIG_COAP_MAX_PDU_SIZE];
};

static void reset_session(struct cli_resources* resources) {
  edhoc_context_deinit(&resources->session.edhoc_ctx);
  if (resources->session.exchange != NULL) {
    resources->session.exchange = NULL;
  }
  memset(&resources->session, 0, sizeof(resources->session));
}

void cli_coap_cleanup_resources(struct cli_resources* resources) {
  if (resources == NULL) {
    return;
  }

  com_cleanup_resources(&resources->infrastructure.common_resources);
  reset_session(resources);
  free(resources);
}

struct cli_resources* cli_resources_create(coap_context_t* context,
                                           coap_session_t* session,
                                           const coap_uri_t uri,
                                           const coap_address_t address) {
  struct cli_resources* resources = calloc(sizeof(struct cli_resources), 1);
  if (!resources) {
    return NULL;
  }
  resources->infrastructure = (struct cli_coap_infrastructure_resources){
      .common_resources = {.coap_context = context},
      .coap_session = session,
      .uri = uri,
      .destination = address,
  };
  return resources;
}

bool cli_resources_setup_session(
    struct cli_resources* resources,
    const struct srv_edhoc_parameters edhoc_parameters) {
  const struct cli_coap_exchange_session_data exchange_data = {
      .context = resources->infrastructure.common_resources.coap_context,
      .session = resources->infrastructure.coap_session,
      .uri = resources->infrastructure.uri,
      .destination = resources->infrastructure.destination,
  };
  const struct com_writable_buffer payload_buf = {
      .bytes = resources->payload_memory,
      .capacity = CONFIG_COAP_MAX_PDU_SIZE,
  };
  resources->session.exchange =
      cli_coap_init_exchange(&exchange_data, payload_buf);
  if (resources->session.exchange == NULL) {
    coap_log_err("Failed to initialize EDHOC exchange\n");
    return false;
  }
  // WHY ARE WE NOT USING THE ERROR BUFFER?
  // This error is before any message is sent so it doesn't make sense to start
  // the communication with an error message
  if (com_edhoc_setup_context(&resources->session.edhoc_ctx, edhoc_parameters,
                              payload_buf)
          .status != COM_EDHOC_SETUP_CTX_OK) {
    coap_log_err("Failed to initialize EDHOC context\n");
    return false;
  }
  return true;
}

struct cli_coap_exchange* cli_resources_get_exchange(
    const struct cli_resources* resources) {
  if (resources == NULL) {
    coap_log_err("cli-resources-get-exchange resource is NULL\n");
    return NULL;
  }
  return resources->session.exchange;
}

struct edhoc_context* cli_resources_get_edhoc_context(
    struct cli_resources* resources) {
  if (resources == NULL) {
    coap_log_err("cli-resources-get-edhoc-context resource is NULL\n");
    return NULL;
  }
  return &resources->session.edhoc_ctx;
}

struct com_writable_buffer cli_resources_get_payload(
    struct cli_resources* resources) {
  if (resources == NULL) {
    coap_log_err("cli-resources-get-payload resource is NULL\n");
    return (struct com_writable_buffer){0};
  }
  return (struct com_writable_buffer){
      .bytes = resources->payload_memory,
      .capacity = sizeof(resources->payload_memory)};
}
