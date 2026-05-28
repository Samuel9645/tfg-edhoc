/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 21/05/2026
 * @brief Definitions for the function to bind the OSCORE session to the
 * current CoAP context using EDHOC
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include <stdio.h>
#include <string.h>

#include "oscore/internal/common/com_oscore_get_configuration.h"
#include "oscore/server/srv_oscore_bind_session.h"

coap_session_t* cli_oscore_create_session(coap_context_t* coap_context,
                                          struct edhoc_context* edhoc_context,
                                          const coap_address_t* destination) {
  char configuration_text[COM_OSCORE_CONFIG_CSV_MAX_SIZE] = {0};
  const bool config_generated = com_oscore_build_config(
      edhoc_context,
      (struct com_writable_char_buffer){
          .data = configuration_text, .capacity = sizeof(configuration_text)});
  if (config_generated == false) {
    coap_log_err(
        "OSCORE Binding: Failed to generate OSCORE configuration from EDHOC "
        "context\n");
    return NULL;
  }
  const coap_str_const_t configuration_data = {
      .length = strlen(configuration_text),
      .s = (const uint8_t*)configuration_text};

  coap_oscore_conf_t* oscore_configuration =
      coap_new_oscore_conf(configuration_data, NULL, NULL, 0);
  if (oscore_configuration == NULL) {
    coap_log_err(
        "OSCORE Create session Error: libcoap failed to parse generated "
        "configuration "
        "text\n");
    return NULL;
  }
  coap_session_t* oscore_session = coap_new_client_session_oscore3(
      coap_context, NULL, destination, COAP_PROTO_UDP, oscore_configuration,
      NULL, NULL, NULL);

  if (oscore_session == NULL) {
    coap_log_err(
        "OSCORE Create session Error: Failed to create secured client "
        "session\n");
    coap_delete_oscore_conf(oscore_configuration);
    return NULL;
  }
  return oscore_session;
}
