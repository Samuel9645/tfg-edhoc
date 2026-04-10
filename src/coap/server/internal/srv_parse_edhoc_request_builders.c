/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Helper functions to build results of parsing EDHOC requests
 */

#include "srv_parse_edhoc_request_builders.h"

struct srv_coap_parse_edhoc_request_result
srv_coap_internal_parse_edhoc_failure(
    const enum srv_coap_parse_edhoc_request_status status) {
  return (struct srv_coap_parse_edhoc_request_result){
      .status = status,
      .parsed_request = {0},
  };
}

struct srv_coap_parse_edhoc_request_result srv_coap_internal_parse_edhoc_ok(
    const struct com_readonly_buffer parsed_data) {
  return (struct srv_coap_parse_edhoc_request_result){
      .status = SRV_COAP_EDH_REQ_OK,
      .parsed_request = parsed_data,
  };
}