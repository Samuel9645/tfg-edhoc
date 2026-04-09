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

struct edh_cli_handshake {
  struct edhoc_context context;
  bool initialized;
};

enum edh_cli_init_status {
  EDH_CLI_INIT_OK = 0,
  EDH_CLI_INIT_ERR_INVALID_ARGS,
  EDH_CLI_INIT_ERR_EDHOC_CONTEXT_SETUP_FAILED,
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
enum edh_cli_init_status edh_cli_init(
    struct edh_cli_handshake* state,
    const struct edhoc_credentials* credentials);

/**
 * @brief Check whether handshake context has been initialized.
 *
 * @param[in] state Handshake state to inspect.
 * @return true when handshake context is initialized, false otherwise.
 */
bool edh_cli_handshake_is_initialized(const struct edh_cli_handshake* state);

/**
 * @brief Deinitialize handshake context and clear state.
 *
 * @param[out] state Handshake state to clear.
 */
void edh_cli_clear_state(struct edh_cli_handshake* state);

#endif  // EDHOC_CLIENT_HANDSHAKE_COMMON_CLI_STATE_H_
