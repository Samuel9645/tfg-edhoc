/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Common server handshake lifecycle state helpers and teardown logic.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/common/state.h"

#include <string.h>

#include "edhoc/common/setup.h"

edh_srv_init_status_t edh_srv_init(
    edh_srv_handshake_t* state, const struct edhoc_credentials* credentials) {
  if (state == NULL) {
    return EDH_SRV_INIT_ERR_INVALID_ARGS;
  }

  memset(state, 0, sizeof(*state));

  if (edh_com_setup_context(&state->context, credentials) != EDHOC_SUCCESS) {
    return EDH_SRV_INIT_ERR_EDHOC_CONTEXT_SETUP_FAILED;
  }

  state->initialized = true;
  return EDH_SRV_INIT_OK;
}

bool edh_srv_handshake_is_initialized(const edh_srv_handshake_t* state) {
  return state != NULL && state->initialized;
}

void edh_srv_clear_state(edh_srv_handshake_t* state) {
  if (!edh_srv_handshake_is_initialized(state)) {
    return;
  }

  (void)edhoc_context_deinit(&state->context);
  memset(state, 0, sizeof(*state));
}
