#ifndef EDHOC_CONFIG_H_
#define EDHOC_CONFIG_H_

#include <stddef.h>
#include <stdint.h>

enum {
  EDC_CBOR_TRUE = 0xF5,
  // Cryptographic constants
  EDC_NONCE_LENGTH_BYTES = 13,
  EDC_AES_CCM_TAG_LENGTH = 16,

  // Key and message constants
  EDC_EXPORTED_SECRET_LENGTH = 32,
  EDC_PLAINTEXT_MAX_LENGTH = 256,
  EDC_CIPHERTEXT_MAX_LENGTH = EDC_PLAINTEXT_MAX_LENGTH + EDC_AES_CCM_TAG_LENGTH,
  EDC_MESSAGE_BUFFER_LENGTH = 512,
};

extern const int32_t EDC_CONNECTION_ID;
extern const size_t EDC_PKR_OUT_LABEL;

#endif  // EDHOC_CONFIG_H_
