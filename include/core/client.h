#ifndef CORE_CLIENT_H_
#define CORE_CLIENT_H_

#include "common/emulation_status.h"

/**
 * @brief Executes EDHOC client handshake and send encrypted message to server
 *
 * @return EMULATION_SUCCESS on success, EMULATION_FAILURE on failure
 */
emulation_status_t tfg_run_client(void);

#endif  // CORE_CLIENT_H_