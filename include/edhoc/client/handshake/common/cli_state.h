/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Common client handshake lifecycle state and teardown API.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_COMMON_CLI_STATE_H_
#define EDHOC_CLIENT_HANDSHAKE_COMMON_CLI_STATE_H_

#include <edhoc.h>
#include <stdbool.h>

struct cli_edhoc_handshake {
  struct edhoc_context context;
  bool initialized;
};

enum cli_edhoc_init_status {
  CLI_EDHOC_INIT_OK = 0,
  CLI_EDHOC_INIT_ERR_INVALID_ARGS,
  CLI_EDHOC_INIT_ERR_EDHOC_CONTEXT_SETUP_FAILED,
};

/**
 * @brief Initialize EDHOC context and mark handshake as initialized.
 *
 * @param[out] state Caller-owned handshake state.
 * @param[in] credentials Credentials for EDHOC context setup.
 * @return Initialization status code.
 *
 * @note This function does not allocate memory with calloc.
 */
enum cli_edhoc_init_status cli_edhoc_init(
    struct cli_edhoc_handshake* state,
    const struct edhoc_credentials* credentials);

/**
 * @brief Check whether handshake context has been initialized.
 *
 * @param[in] state Handshake state to inspect.
 * @return true when handshake context is initialized, false otherwise.
 */
bool cli_edhoc_handshake_is_initialized(
    const struct cli_edhoc_handshake* state);

/**
 * @brief Deinitialize handshake context and clear state.
 *
 * @param[out] state Handshake state to clear.
 */
void cli_edhoc_clear_state(struct cli_edhoc_handshake* state);

#endif  // EDHOC_CLIENT_HANDSHAKE_COMMON_CLI_STATE_H_
