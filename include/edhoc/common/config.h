#ifndef EDHOC_COMMON_CONFIG_H_
#define EDHOC_COMMON_CONFIG_H_

#include <stddef.h>
#include <stdint.h>

enum {
  // Cryptographic constants
  NONCE_LENGTH_BYTES = 13,
  AES_CCM_TAG_LENGTH = 16,

  // Key and message constants
  EXPORTED_SECRET_LENGTH = 32,
  PLAINTEXT_MAX_LENGTH = 256,
  CIPHERTEXT_MAX_LENGTH = PLAINTEXT_MAX_LENGTH + AES_CCM_TAG_LENGTH,
  MESSAGE_BUFFER_LENGTH = 512,
};

extern const int32_t CONNECTION_ID;
extern const size_t PKR_OUT_LABEL;

#endif  // EDHOC_COMMON_CONFIG_H_
