/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Test payload definitions for EDHOC server handshake unit tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_SERVER_HANDSHAKE_PAYLOAD_H_
#define EDHOC_SERVER_HANDSHAKE_PAYLOAD_H_

#include <stddef.h>
#include <stdint.h>

enum { EDH_SRV_HND_PAY_LEN = 8 };

/**
 * @brief Simple struct to hold test payload data and its length.
 */
typedef struct tst_edh_payload_t {
  uint8_t data[EDH_SRV_HND_PAY_LEN];
  size_t length;
} tst_edh_payload_t;

/**
 * @brief Generates a test payload with the correct CBOR TRUE prefix for
 * Message 1.
 * @return Payload containing the invalid Message 1.
 */
tst_edh_payload_t get_invalid_prefix_payload(void);

/**
 * @brief Generates a valid test payload for Message 1 with the correct CBOR
 * TRUE prefix.
 * @return Payload containing the valid Message 1.
 */
tst_edh_payload_t get_valid_message_1_payload(void);

#endif  // EDHOC_SERVER_HANDSHAKE_PAYLOAD_H_