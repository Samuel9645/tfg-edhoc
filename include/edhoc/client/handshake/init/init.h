/**
 * @file init.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Initialization API for client handshake initialization step.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_INIT_INIT_H_
#define EDHOC_CLIENT_HANDSHAKE_INIT_INIT_H_

#include "edhoc/client/handshake/common/state.h"
#include "edhoc/client/handshake/init/result.h"

/**
 * @brief Initialize EDHOC context and mark handshake as initialized.
 *
 * @param[out] state Caller-owned handshake state.
 * @return Initialization status code.
 *
 * @note This function does not allocate memory with calloc.
 */
edh_cli_init_status_t edh_cli_init(edh_cli_handshake_t* state);

#endif  // EDHOC_CLIENT_HANDSHAKE_INIT_INIT_H_
