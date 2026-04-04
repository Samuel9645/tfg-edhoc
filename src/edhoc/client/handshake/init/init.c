/**
 * @file init.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Initialization implementation for client handshake initialization
 * step.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/init/init.h"

#include <edhoc_helpers.h>
#include <string.h>

#include "edhoc/common/setup.h"
#include "edhoc/credentials/authentication.h"
#include "edhoc/credentials/client_private_key.h"
#include "edhoc/credentials/public_data.h"

static int client_credential_fetch(void* user_context,
                                   struct edhoc_auth_creds* credentials) {
  return edh_cred_auth_fetch(
      user_context, credentials, EDH_CRED_PUB_CLI_PK,
      ARRAY_SIZE(EDH_CRED_PUB_CLI_PK), EDH_CRED_CLI_PRIVATE_KEY,
      ARRAY_SIZE(EDH_CRED_CLI_PRIVATE_KEY), EDH_CRED_PUB_CLI_KID);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef (libedhoc signature
// forces it)
static int client_credential_verify(void* user_context,
                                    struct edhoc_auth_creds* credentials,
                                    const uint8_t** public_key_reference,
                                    size_t* public_key_length) {
  return edh_cred_auth_verify(
      user_context, credentials, EDH_CRED_PUB_SRV_KID, EDH_CRED_PUB_SRV_PK,
      ARRAY_SIZE(EDH_CRED_PUB_SRV_PK), public_key_reference, public_key_length);
}

edh_cli_init_status_t edh_cli_init(edh_cli_handshake_t* state) {
  if (state == NULL) {
    return EDH_CLI_INIT_ERR_INVALID_ARGS;
  }

  memset(state, 0, sizeof(*state));

  const struct edhoc_credentials credentials = {
      .fetch = client_credential_fetch,
      .verify = client_credential_verify,
  };

  if (edh_com_setup_context(&state->context, &credentials) != EDHOC_SUCCESS) {
    return EDH_CLI_INIT_ERR_EDHOC_CONTEXT_SETUP_FAILED;
  }

  state->initialized = true;
  return EDH_CLI_INIT_OK;
}
