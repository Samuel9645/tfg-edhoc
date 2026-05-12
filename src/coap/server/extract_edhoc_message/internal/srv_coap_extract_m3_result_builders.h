/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 12/05/2026
 * @brief Declarations for the srv_coap_extract_m3_result_builders module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef COAP_SERVER_EXTRACT_EDHOC_MESSAGE_INTERNAL_SRV_COAP_EXTRACT_M3_RESULT_BUILDERS_H_
#define COAP_SERVER_EXTRACT_EDHOC_MESSAGE_INTERNAL_SRV_COAP_EXTRACT_M3_RESULT_BUILDERS_H_

#include "coap/server/extract_edhoc_message/srv_coap_extract_m3.h"

struct srv_coap_extract_message_3_result srv_coap_parse_message_3_ok(
    struct com_readonly_buffer buffer);

struct srv_coap_extract_message_3_result srv_coap_parse_message_3_failure(
    enum srv_coap_extract_message_3_status status,
    struct com_readonly_buffer error_buffer);

struct srv_coap_extract_message_3_result
srv_coap_parse_message_3_invalid_error_buffer(void);

#endif  // COAP_SERVER_EXTRACT_EDHOC_MESSAGE_INTERNAL_SRV_COAP_EXTRACT_M3_RESULT_BUILDERS_H_

