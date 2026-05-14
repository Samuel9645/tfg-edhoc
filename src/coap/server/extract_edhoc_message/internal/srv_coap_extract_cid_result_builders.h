/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 14/05/2026
 * @brief Declarations for the srv_coap_extract_cid_result_builders module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef COAP_SERVER_EXTRACT_EDHOC_MESSAGE_INTERNAL_SRV_COAP_EXTRACT_CID_RESULT_BUILDERS_H_
#define COAP_SERVER_EXTRACT_EDHOC_MESSAGE_INTERNAL_SRV_COAP_EXTRACT_CID_RESULT_BUILDERS_H_

#include "coap/server/extract_edhoc_message/srv_coap_extract_m3.h"

struct srv_coap_extract_connection_id_result srv_coap_extract_connection_id_ok(
    struct edhoc_connection_id cid, struct com_readonly_buffer message_payload);

struct srv_coap_extract_connection_id_result
srv_coap_extract_connection_id_failure(enum srv_coap_extract_cid_status status);

#endif  // COAP_SERVER_EXTRACT_EDHOC_MESSAGE_INTERNAL_SRV_COAP_EXTRACT_CID_RESULT_BUILDERS_H_
