/**
 * @file state.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Common client handshake lifecycle state helpers and teardown logic.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/common/state.h"

#include <string.h>

bool edh_cli_handshake_is_initialized(const edh_cli_handshake_t* state) {
  return state != NULL && state->initialized;
}

void edh_cli_clear_state(edh_cli_handshake_t* state) {
  if (!edh_cli_handshake_is_initialized(state)) {
    return;
  }

  (void)edhoc_context_deinit(&state->context);
  memset(state, 0, sizeof(*state));
}
