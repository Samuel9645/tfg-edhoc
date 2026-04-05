/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Helper functions to build results of parsing EDHOC requests
 */

#ifndef COAP_SERVER_INTERNAL_PARSE_EDHOC_RESULT_BUILDER_H_
#define COAP_SERVER_INTERNAL_PARSE_EDHOC_RESULT_BUILDER_H_

#include "coap/server/srv_parse_edhoc_request.h"

struct cp_srv_parse_edhoc_request_result cp_srv_internal_parse_edhoc_failure(
    enum cp_srv_parse_edhoc_request_status status);

struct cp_srv_parse_edhoc_request_result cp_srv_internal_parse_edhoc_ok(
    struct com_readonly_buffer parsed_data);

#endif  // COAP_SERVER_INTERNAL_PARSE_EDHOC_RESULT_BUILDER_H_