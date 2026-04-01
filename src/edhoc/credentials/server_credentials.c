#include "edhoc/credentials/server_credentials.h"

#include "edhoc/credentials/authentication.h"
#include "edhoc/credentials/public_data.h"
#include "edhoc/credentials/server_private_key.h"

static int server_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  return edh_cred_auth_fetch(
      user_context, credentials, EDH_CRED_PUB_SRV_PK,
      ARRAY_SIZE(EDH_CRED_PUB_SRV_PK), EDH_CRED_SRV_PRIVATE_KEY,
      ARRAY_SIZE(EDH_CRED_SRV_PRIVATE_KEY), EDH_CRED_PUB_SRV_KID);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
static int server_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  return edh_cred_auth_verify(
      user_context, credentials, EDH_CRED_PUB_CLI_KID, EDH_CRED_PUB_CLI_PK,
      ARRAY_SIZE(EDH_CRED_PUB_CLI_PK), public_key_reference, public_key_length);
}

const struct edhoc_credentials EDH_CRED_SRV_CRED = {
    .fetch = server_credential_fetch,
    .verify = server_credential_verify,
};