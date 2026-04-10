/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Helper functions to build results of parsing EDHOC requests
 */

#ifndef COAP_SERVER_INTERNAL_PARSE_EDHOC_RESULT_BUILDER_H_
#define COAP_SERVER_INTERNAL_PARSE_EDHOC_RESULT_BUILDER_H_

#include "coap/server/srv_parse_edhoc_request.h"

struct srv_coap_parse_edhoc_request_result
srv_coap_internal_parse_edhoc_failure(
    enum srv_coap_parse_edhoc_request_status status);

struct srv_coap_parse_edhoc_request_result srv_coap_internal_parse_edhoc_ok(
    struct com_readonly_buffer parsed_data);

#endif  // COAP_SERVER_INTERNAL_PARSE_EDHOC_RESULT_BUILDER_H_