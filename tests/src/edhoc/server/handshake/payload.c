/**
 * @file payload.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Test payload factories for EDHOC server handshake tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/payload.h"

#include <string.h>

#include "edhoc/common/constants.h"

static tst_edh_payload_t create_payload(const uint8_t prefix) {
  tst_edh_payload_t payload;
  memset(payload.data, 0, EDH_SRV_HND_PAY_LEN);
  payload.data[0] = prefix;
  payload.length = EDH_SRV_HND_PAY_LEN;
  return payload;
}

tst_edh_payload_t get_invalid_prefix_payload(void) {
  return create_payload(0xAA);
}

tst_edh_payload_t get_valid_message_1_payload(void) {
  return create_payload(EDH_COM_CBOR_TRUE);
}