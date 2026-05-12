/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 12/05/2026
 * @brief Declarations for the srv_coap_m1_parse_result_builders module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef COAP_SERVER_EDHOC_PARSE_INTERNAL_SRV_COAP_M1_PARSE_RESULT_BUILDERS_H_
#define COAP_SERVER_EDHOC_PARSE_INTERNAL_SRV_COAP_M1_PARSE_RESULT_BUILDERS_H_

#include "coap/server/extract_edhoc_message/srv_coap_extract_m1.h"

struct srv_coap_extract_message_1_result srv_coap_parse_message_1_ok(
    struct com_readonly_buffer buffer);

struct srv_coap_extract_message_1_result srv_coap_parse_message_1_failure(
    enum srv_coap_extract_message_1_status status,
    struct com_readonly_buffer error_buffer);

struct srv_coap_extract_message_1_result
srv_coap_parse_message_1_invalid_error_buffer(void);

#endif  // COAP_SERVER_EDHOC_PARSE_INTERNAL_SRV_COAP_M1_PARSE_RESULT_BUILDERS_H_
