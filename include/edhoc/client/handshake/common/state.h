/**
 * @file state.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Common client handshake lifecycle state and teardown API.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_COMMON_STATE_H_
#define EDHOC_CLIENT_HANDSHAKE_COMMON_STATE_H_

#include <edhoc.h>
#include <stdbool.h>

/**
 * @brief EDHOC client handshake state.
 */
typedef struct edh_cli_handshake {
  struct edhoc_context context;
  bool initialized;
} edh_cli_handshake_t;

/**
 * @brief Check whether handshake context has been initialized.
 *
 * @param[in] state Handshake state to inspect.
 * @return true when handshake context is initialized, false otherwise.
 */
bool edh_cli_handshake_is_initialized(const edh_cli_handshake_t* state);

/**
 * @brief Deinitialize handshake context and clear state.
 *
 * @param[out] state Handshake state to clear.
 */
void edh_cli_clear_state(edh_cli_handshake_t* state);

#endif  // EDHOC_CLIENT_HANDSHAKE_COMMON_STATE_H_
