#ifndef SERVER_H_
#define SERVER_H_

#include "common/emulation_status.h"

/**
 * @brief Execute EDHOC server handshake and receive encrypted message from
 * client
 *
 * @return EMULATION_SUCCESS on success, EMULATION_FAILURE on failure
 */
emulation_status_t run_server(void);

#endif  // SERVER_H_