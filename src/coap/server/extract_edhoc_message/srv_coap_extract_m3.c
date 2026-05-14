/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 12/05/2026
 * @brief Module to extract EDHOC Message 3 from the request payload.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "coap/server/extract_edhoc_message/srv_coap_extract_m3.h"

#include <edhoc_helpers.h>

#include "coap/server/extract_edhoc_message/internal/srv_coap_extract_cid_result_builders.h"
#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"

struct srv_coap_extract_connection_id_result srv_coap_extract_connection_id(
    const struct com_readonly_buffer request_buffer) {
  if (!com_readonly_buffer_has_content(request_buffer)) {
    return srv_coap_extract_connection_id_failure(
        SRV_COAP_EXTRACT_CID_ERR_EMPTY_BUFFER);
  }

  struct edhoc_extracted_fields extracted_fields = {
      .buffer = request_buffer.bytes,
      .buffer_size = request_buffer.length,
      .edhoc_message_ptr = request_buffer.bytes,
      .edhoc_message_size = request_buffer.length,
  };
  if (edhoc_extract_connection_id(&extracted_fields) != EDHOC_SUCCESS) {
    return srv_coap_extract_connection_id_failure(
        SRV_COAP_EXTRACT_CID_ERR_EXTRACT);
  }

  return srv_coap_extract_connection_id_ok(
      extracted_fields.extracted_conn_id,
      (struct com_readonly_buffer){
          .bytes = extracted_fields.edhoc_message_ptr,
          .length = extracted_fields.edhoc_message_size,
      });
}

bool srv_coap_connection_id_is_expected(
    const struct edhoc_connection_id* extracted_cid,
    const struct edhoc_context* edhoc_ctx) {
  if (edhoc_ctx == NULL) {
    return false;
  }
  return edhoc_connection_id_equal(extracted_cid, &edhoc_ctx->private_cid);
}
