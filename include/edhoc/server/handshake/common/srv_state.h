/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Common server handshake lifecycle state and teardown API.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_COMMON_STATE_H_
#define EDHOC_SERVER_HANDSHAKE_COMMON_STATE_H_

#include <edhoc.h>
#include <stdbool.h>

/**
 * @brief EDHOC server handshake state.
 */
typedef struct edh_srv_handshake {
  struct edhoc_context context;
  bool initialized;
} edh_srv_handshake_t;

/**
 * @brief Status codes for server handshake initialization.
 */
typedef enum edh_srv_init_status {
  EDH_SRV_INIT_OK = 0,
  EDH_SRV_INIT_ERR_INVALID_ARGS,
  EDH_SRV_INIT_ERR_EDHOC_CONTEXT_SETUP_FAILED,
} edh_srv_init_status_t;

/**
 * @brief Initialize EDHOC context and mark handshake as initialized.
 *
 * @param[out] state Caller-owned handshake state.
 * @param[in] credentials Credentials for EDHOC context setup.
 * @return Initialization status code.
 *
 * @note This function does not allocate memory with calloc.
 */
edh_srv_init_status_t edh_srv_init(edh_srv_handshake_t* state,
                                   const struct edhoc_credentials* credentials);

/**
 * @brief Check whether handshake context has been initialized.
 *
 * @param[in] state Handshake state to inspect.
 * @return true when handshake context is initialized, false otherwise.
 */
bool edh_srv_handshake_is_initialized(const edh_srv_handshake_t* state);

/**
 * @brief Deinitialize handshake context and clear state.
 *
 * @param[out] state Handshake state to clear.
 */
void edh_srv_clear_state(edh_srv_handshake_t* state);

#endif  // EDHOC_SERVER_HANDSHAKE_COMMON_STATE_H_
