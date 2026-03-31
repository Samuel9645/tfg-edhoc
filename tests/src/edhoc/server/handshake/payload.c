#include "edhoc/server/handshake/payload.h"

#include "edhoc/common/constants.h"

static test_edsh_payload_t create_payload(const uint8_t prefix) {
  test_edsh_payload_t payload;
  memset(payload.data, 0, BUFFER_SIZE);
  payload.data[0] = prefix;
  payload.length = BUFFER_SIZE;
  return payload;
}

test_edsh_payload_t get_invalid_prefix_payload(void) {
  return create_payload(0xAA);
}

test_edsh_payload_t get_valid_message_1_payload(void) {
  return create_payload(EDCC_CBOR_TRUE);
}