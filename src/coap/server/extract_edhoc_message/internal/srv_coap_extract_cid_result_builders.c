/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 14/05/2026
 * @brief Definitions for the srv_coap_extract_cid_result_builders module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/server/extract_edhoc_message/internal/srv_coap_extract_cid_result_builders.h"

struct srv_coap_extract_connection_id_result srv_coap_extract_connection_id_ok(
    const struct edhoc_connection_id cid,
    const struct com_readonly_buffer message_payload) {
  return (struct srv_coap_extract_connection_id_result){
      .status = SRV_COAP_EXTRACT_CID_OK,
      .cid = cid,
      .message_payload = message_payload,
  };
}

struct srv_coap_extract_connection_id_result
srv_coap_extract_connection_id_failure(
    const enum srv_coap_extract_cid_status status) {
  return (struct srv_coap_extract_connection_id_result){
      .status = status,
  };
}
