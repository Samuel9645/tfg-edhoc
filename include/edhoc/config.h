#ifndef EDHOC_CONFIG_H_
#define EDHOC_CONFIG_H_

#include <stddef.h>
#include <stdint.h>

enum {
  // Cryptographic constants
  EDH_CFG_NONCE_LENGTH_BYTES = 13,
  EDH_CFG_AES_CCM_TAG_LENGTH = 16,

  // Key and message constants
  EDH_CFG_EXPORTED_SECRET_LENGTH = 32,
  EDH_CFG_PLAINTEXT_MAX_LENGTH = 256,
  EDH_CFG_CIPHERTEXT_MAX_LENGTH =
      EDH_CFG_PLAINTEXT_MAX_LENGTH + EDH_CFG_AES_CCM_TAG_LENGTH,
  EDH_CFG_MESSAGE_BUFFER_LENGTH = 512,
};

extern const int32_t EDH_CFG_CONNECTION_ID;
extern const size_t EDH_CFG_PKR_OUT_LABEL;

#endif  // EDHOC_CONFIG_H_
