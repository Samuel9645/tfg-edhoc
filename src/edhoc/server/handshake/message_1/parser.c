/**
 * @file parser.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Parsing helpers for EDHOC server Message 1.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/parser.h"

#include <edhoc/common/constants.h>
#include <stdbool.h>

static bool arguments_are_invalid(const uint8_t** payload,
                                  const size_t* length) {
  return !payload || !*payload || !length || *length == 0;
}

static bool first_byte_is_not_cbor_true(const uint8_t** payload) {
  return (*payload)[0] != EDH_COM_CBOR_TRUE;
}

edh_srv_message_1_handler_status_t edh_srv_remove_cbor_true_prefix(
    const uint8_t** payload, size_t* length) {
  if (arguments_are_invalid(payload, length)) {
    return EDH_SRV_MSG1_HDL_ERR_INVALID_ARGS;
  }
  if (first_byte_is_not_cbor_true(payload)) {
    return EDH_SRV_MSG1_HDL_ERR_PREFIX_MISSING;
  }

  *payload += 1;
  *length -= 1;
  return EDH_SRV_MSG1_HDL_OK;
}

bool edh_srv_parse_message_1(const uint8_t* request_payload,
                             const size_t request_len,
                             com_readonly_buffer_t* parsed_payload) {
  // Validate framing and return a borrowed view of Message 1 without the
  // leading CBOR TRUE marker. No payload bytes are copied.
  if (!request_payload || request_len == 0 || !parsed_payload) {
    return false;
  }

  const uint8_t* payload_ptr = request_payload;
  size_t payload_len = request_len;
  if (edh_srv_remove_cbor_true_prefix(&payload_ptr, &payload_len) !=
      EDH_SRV_MSG1_HDL_OK) {
    return false;
  }

  *parsed_payload = (com_readonly_buffer_t){
      .bytes = payload_ptr,
      .length = payload_len,
  };
  return true;
}
