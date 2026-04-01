#ifndef EDHOC_CREDENTIALS_PUBLIC_DATA_H_
#define EDHOC_CREDENTIALS_PUBLIC_DATA_H_

#include <stdint.h>

enum { EDH_CRED_PUB_PK_LENGTH = 65 };

extern const uint8_t EDH_CRED_PUB_CLI_PK[EDH_CRED_PUB_PK_LENGTH];
extern const uint8_t EDH_CRED_PUB_SRV_PK[EDH_CRED_PUB_PK_LENGTH];
extern const int32_t EDH_CRED_PUB_SRV_KID;
extern const int32_t EDH_CRED_PUB_CLI_KID;
extern const uint16_t EDH_CRED_PUB_SRV_PORT;
extern const char EDH_CRED_PUB_SRV_URI[];

#endif // EDHOC_CREDENTIALS_PUBLIC_DATA_H_
