/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Helper functions to build results of parsing EDHOC requests
 */

#include "parse_edhoc_result_builders.h"

struct cp_srv_parse_edhoc_request_result cp_srv_internal_parse_edhoc_failure(
    const enum cp_srv_parse_edhoc_request_status status) {
  return (struct cp_srv_parse_edhoc_request_result){
      .status = status,
      .parsed_request = {0},
  };
}

struct cp_srv_parse_edhoc_request_result cp_srv_internal_parse_edhoc_ok(
    const struct com_readonly_buffer parsed_data) {
  return (struct cp_srv_parse_edhoc_request_result){
      .status = CP_SRV_EDH_REQ_OK,
      .parsed_request = parsed_data,
  };
}