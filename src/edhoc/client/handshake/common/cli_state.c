/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Common client handshake lifecycle state helpers and teardown logic.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/common/cli_state.h"

#include <string.h>

#include "edhoc/common/com_edhoc_context_setup.h"

enum cli_edhoc_init_status cli_edhoc_init(
    struct cli_edhoc_handshake* state,
    const struct edhoc_credentials* credentials) {
  if (state == NULL) {
    return CLI_EDHOC_INIT_ERR_INVALID_ARGS;
  }

  memset(state, 0, sizeof(*state));

  if (com_edhoc_setup_context(&state->context, credentials) != EDHOC_SUCCESS) {
    return CLI_EDHOC_INIT_ERR_EDHOC_CONTEXT_SETUP_FAILED;
  }

  state->initialized = true;
  return CLI_EDHOC_INIT_OK;
}

bool cli_edhoc_handshake_is_initialized(
    const struct cli_edhoc_handshake* state) {
  if (state == NULL) {
    return false;
  }
  return state->initialized;
}

void cli_edhoc_clear_state(struct cli_edhoc_handshake* state) {
  if (!cli_edhoc_handshake_is_initialized(state)) {
    return;
  }

  (void)edhoc_context_deinit(&state->context);
  memset(state, 0, sizeof(*state));
}
