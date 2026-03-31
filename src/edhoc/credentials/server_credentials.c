#include "edhoc/credentials/server_credentials.h"

#include "edhoc/credentials/authentication.h"
#include "edhoc/credentials/public_data.h"
#include "edhoc/credentials/server_private_key.h"

static int server_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  return edhoc_credentials_fetch(user_context, credentials, SERVER_PUBLIC_KEY,
                                 ARRAY_SIZE(SERVER_PUBLIC_KEY),
                                 SERVER_PRIVATE_KEY,
                                 ARRAY_SIZE(SERVER_PRIVATE_KEY), SERVER_KID);
}

static int server_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  return edhoc_credentials_verify(
      user_context, credentials, CLIENT_KID, CLIENT_PUBLIC_KEY,
      ARRAY_SIZE(CLIENT_PUBLIC_KEY), public_key_reference, public_key_length);
}

const struct edhoc_credentials SERVER_CREDENTIALS = {
    .fetch = server_credential_fetch,
    .verify = server_credential_verify,
};