// ReSharper disable CppClassNeverUsed
#include "coap/client/cli_resources.h"

#include <string.h>

#include "common/com_session_resources.h"
#include "edhoc/common/com_edhoc_manage_context.h"

static bool reset_edhoc_context(struct cli_resources* resources) {
  if (resources == NULL) {
    coap_log_err("resources is NULL\n");
    return false;
  }
  edhoc_context_deinit(&resources->edhoc_context);
  return true;
}

bool cli_reset_edhoc_context_with_new_suites_data(
    struct cli_resources* resources,
    const struct com_edhoc_parameters original_parameters,
    const struct com_edhoc_cipher_suite_details* selected_cipher_suite,
    const struct cli_edhoc_renegotiation_list negotiated_suites) {
  if (!reset_edhoc_context(resources)) {
    coap_log_err("Failed to reset EDHOC context\n");
    return false;
  }
  const struct com_edhoc_parameters new_parameters = {
      .credentials = original_parameters.credentials,
      .methods = original_parameters.methods,
      .supported_cipher_suites =
          {
              .number_of_suites = negotiated_suites.number_of_suites,
              .suites = negotiated_suites.suites,
          },
      .selected_cipher_suite = selected_cipher_suite,
      .generate_connection_id = original_parameters.generate_connection_id};
  return cli_initialize_edhoc_context_with_parameters(resources,
                                                      new_parameters);
}

bool cli_initialize_edhoc_context_with_parameters(
    struct cli_resources* resources,
    const struct com_edhoc_parameters parameters) {
  if (resources == NULL) {
    coap_log_err("resources is NULL\n");
    return false;
  }
  if (com_edhoc_setup_context(&resources->edhoc_context, parameters).status !=
      COM_EDHOC_SETUP_CTX_OK) {
    coap_log_err("Failed to initialize EDHOC context\n");
    return false;
  }
  return true;
}

void cli_cleanup_resources(struct cli_resources* resources) {
  if (resources == NULL) {
    return;
  }

  cli_coap_cleanup_exchange(resources->exchange);
  if (resources->exchange_session != NULL) {
    coap_session_release(resources->exchange_session);
  }
  if (resources->oscore_session != NULL) {
    coap_session_release(resources->oscore_session);
  }
  if (resources->coap_context != NULL) {
    coap_free_context(resources->coap_context);
  }
  reset_edhoc_context(resources);
}

struct com_writable_buffer cli_resources_get_payload(
    struct cli_resources* resources) {
  if (resources == NULL) {
    coap_log_err("cli-resources-get-payload resource is NULL\n");
    return (struct com_writable_buffer){0};
  }
  return (struct com_writable_buffer){.bytes = resources->payload,
                                      .capacity = sizeof(resources->payload)};
}
