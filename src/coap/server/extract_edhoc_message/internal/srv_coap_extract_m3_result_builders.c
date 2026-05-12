/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 12/05/2026
 * @brief Definitions for the srv_coap_extract_m3_result_builders module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/server/extract_edhoc_message/internal/srv_coap_extract_m3_result_builders.h"

struct srv_coap_extract_message_3_result srv_coap_parse_message_3_ok(
    const struct com_readonly_buffer buffer) {
  return (struct srv_coap_extract_message_3_result){
      .status = SRV_COAP_EXTRACT_MSG3_OK,
      .buffer = buffer,
  };
}

struct srv_coap_extract_message_3_result srv_coap_parse_message_3_failure(
    const enum srv_coap_extract_message_3_status status,
    const struct com_readonly_buffer error_buffer) {
  return (struct srv_coap_extract_message_3_result){
      .status = status,
      .buffer = error_buffer,
  };
}

struct srv_coap_extract_message_3_result
srv_coap_parse_message_3_invalid_error_buffer(void) {
  return (struct srv_coap_extract_message_3_result){
      .status = SRV_COAP_EXTRACT_MSG3_ERR_INVALID_ERROR_BUFFER,
  };
}

