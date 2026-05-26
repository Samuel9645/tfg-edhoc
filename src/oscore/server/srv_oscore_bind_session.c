/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 21/05/2026
 * @brief Definitions for the function to bind the OSCORE session to the
 * current CoAP context using EDHOC
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "oscore/server/srv_oscore_bind_session.h"

#include <string.h>

#include "oscore/internal/common/com_oscore_get_configuration.h"

enum status_coap srv_oscore_bind_session(coap_context_t* coap_context,
                                         struct edhoc_context* edhoc_context) {
  char configuration_text[COM_OSCORE_CONFIG_CSV_MAX_SIZE] = {0};
  const bool config_generated = com_oscore_build_config(
      edhoc_context,
      (struct com_writable_char_buffer){
          .data = configuration_text, .capacity = sizeof(configuration_text)});
  if (config_generated == false) {
    coap_log_err(
        "OSCORE Binding: Failed to generate OSCORE configuration from EDHOC "
        "context\n");
    return STATUS_COAP_ERR;
  }

  const coap_str_const_t* configuration_data =
      coap_make_str_const(configuration_text);

  coap_oscore_conf_t* oscore_configuration =
      coap_new_oscore_conf(*configuration_data, NULL, NULL, 0);
  if (oscore_configuration == NULL) {
    coap_log_err(
        "OSCORE Binding: libcoap failed to parse generated configuration "
        "text\n");
    return STATUS_COAP_ERR;
  }
  if (coap_context_oscore_server(coap_context, oscore_configuration) != 1) {
    coap_log_err(
        "OSCORE Binding: Failed to bind OSCORE configuration to the server "
        "context\n");
    coap_delete_oscore_conf(oscore_configuration);
    return STATUS_COAP_ERR;
  }
  coap_log_info("OSCORE Context successfully binded.\n");
  return STATUS_COAP_OK;
}
