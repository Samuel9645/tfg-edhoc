/**
 * @file parser.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Parsing helpers for EDHOC server Message 3.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <edhoc.h>
#include <edhoc_helpers.h>
#include <edhoc_values.h>
#include <stdbool.h>

#include "edhoc/server/handshake/message_3/srv_m3_parser.h"

bool edh_srv_parse_message_3(const uint8_t* request_payload,
                             const size_t request_len,
                             const struct edhoc_context* edhoc_ctx,
                             struct edhoc_extracted_fields* extracted_fields) {
  // Parse Message 3 as a non-mutating step: populate extracted fields,
  // extract the peer connection ID, and confirm it matches this session.
  if (!request_payload || request_len == 0 || !edhoc_ctx || !extracted_fields) {
    return false;
  }

  *extracted_fields = (struct edhoc_extracted_fields){
      .buffer = request_payload,
      .buffer_size = request_len,
      .edhoc_message_ptr = request_payload,
      .edhoc_message_size = request_len,
  };

  if (edhoc_extract_connection_id(extracted_fields) != EDHOC_SUCCESS) {
    return false;
  }

  return edhoc_connection_id_equal(&extracted_fields->extracted_conn_id,
                                   &edhoc_ctx->private_cid);
}
