#ifndef CORE_CLIENT_H_
#define CORE_CLIENT_H_

#include "common/emulation_status.h"

/**
 * @brief Executes EDHOC client handshake and send encrypted message to server
 *
 * @return COM_EMULATION_SUCCESS on success, COM_EMULATION_FAILURE on failure
 */
com_emulation_status_t core_run_client(void);

#endif  // CORE_CLIENT_H_