#ifndef PUBLIC_DATA_H_
#define PUBLIC_DATA_H_

#include <stddef.h>
#include <stdint.h>

#define CLIENT_PUBLIC_KEY_LEN 65
#define SERVER_PUBLIC_KEY_LEN 65
#define SERVER_PORT 8080
#define SERVER_URI "127.0.0.1"

extern const uint8_t CLIENT_PUBLIC_KEY[CLIENT_PUBLIC_KEY_LEN];
extern const uint8_t SERVER_PUBLIC_KEY[SERVER_PUBLIC_KEY_LEN];

extern const int32_t SERVER_KID;
extern const int32_t CLIENT_KID;
extern const int32_t CONNECTION_ID;

#endif  // PUBLIC_DATA_H_