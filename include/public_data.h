#ifndef PUBLIC_DATA_H_
#define PUBLIC_DATA_H_

#include <stdint.h>

enum { PRE_GENERATED_PUBLIC_KEY_LEN = 65 };
extern const uint8_t CLIENT_PUBLIC_KEY[PRE_GENERATED_PUBLIC_KEY_LEN];
extern const uint8_t SERVER_PUBLIC_KEY[PRE_GENERATED_PUBLIC_KEY_LEN];

#endif  // PUBLIC_DATA_H_