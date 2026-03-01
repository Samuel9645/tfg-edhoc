#ifndef PUBLIC_DATA_H_
#define PUBLIC_DATA_H_

#include <stddef.h>
#include <stdint.h>

enum {
  PRE_GENERATED_PUBLIC_KEY_LEN = 65,
  PKR_EXPORT_SECRET_LEN = 32,
  // plaintext message length + GCM tag length
  CYPERTEXT_LEN = 256 + 16,
  SOCKET_MESSAGE_BUFFER_LEN = 512
};

extern const uint8_t CLIENT_PUBLIC_KEY[PRE_GENERATED_PUBLIC_KEY_LEN];
extern const uint8_t SERVER_PUBLIC_KEY[PRE_GENERATED_PUBLIC_KEY_LEN];

extern const int32_t SERVER_KID;
extern const int32_t CLIENT_KID;
extern const int32_t CONNECTION_ID;
extern const uint16_t SERVER_PORT;
extern const char* SERVER_URI;

#endif  // PUBLIC_DATA_H_