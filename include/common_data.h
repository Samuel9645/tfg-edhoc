#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include <edhoc_values.h>
#include <stddef.h>
#include <stdint.h>

enum {

  PKR_EXPORT_SECRET_LEN = 32,
  // plaintext message length + GCM tag length
  CYPERTEXT_LEN = 256 + 16,
  SOCKET_MESSAGE_BUFFER_LEN = 512
};

extern const int32_t SERVER_KID;
extern const int32_t CLIENT_KID;
extern const int32_t CONNECTION_ID;
extern const uint16_t SERVER_PORT;
extern const char* SERVER_URI;
extern const size_t PKR_OUT_LABEL;

#endif  // COMMON_DATA_H_