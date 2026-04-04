/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Helper functions to build results of parsing EDHOC requests
 */

#include "coap/server/srv_parse_edhoc_request.h"

cp_srv_parse_edhoc_request_result_t cp_srv_internal_parse_edhoc_failure(
    const cp_srv_parse_edhoc_request_status_t status) {
  return (cp_srv_parse_edhoc_request_result_t){
      .status = status,
      .parsed_response = {0},
  };
}

cp_srv_parse_edhoc_request_result_t cp_srv_internal_parse_edhoc_ok(
    const com_readonly_buffer_t parsed_data) {
  return (cp_srv_parse_edhoc_request_result_t){
      .status = CP_SRV_EDH_REQ_OK,
      .parsed_response = parsed_data,
  };
}