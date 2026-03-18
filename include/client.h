#ifndef CLIENT_H_
#define CLIENT_H_

#include "common/emulation_status.h"

/**
 * @brief Executes EDHOC client handshake and send encrypted message to server
 *
 * @return EMULATION_SUCCESS on success, EMULATION_FAILURE on failure
 */
emulation_status_t run_client(void);

#endif  // CLIENT_H_