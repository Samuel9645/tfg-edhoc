#include "edhoc/credentials/cred_srv.h"

#include "edhoc/credentials/cred_auth.h"
#include "edhoc/credentials/cred_pub_data.h"
#include "edhoc/credentials/cred_srv_key.h"

static int server_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  return cred_edhoc_auth_fetch(
      user_context, credentials, CRED_EDHOC_PUB_SRV_PK,
      CRED_EDHOC_PUB_PK_LENGTH, CRED_EDHOC_SRV_PRIVATE_KEY,
      CRED_EDHOC_SRV_PRIVATE_KEY_LENGTH, CRED_EDHOC_PUB_SRV_KID);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
static int server_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  return cred_edhoc_auth_verify(user_context, credentials,
                                CRED_EDHOC_PUB_CLI_KID, CRED_EDHOC_PUB_CLI_PK,
      CRED_EDHOC_PUB_PK_LENGTH, public_key_reference, public_key_length);
}

const struct edhoc_credentials CRED_EDHOC_SRV = {
    .fetch = server_credential_fetch,
    .verify = server_credential_verify,
};