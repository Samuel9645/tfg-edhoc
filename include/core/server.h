#ifndef CORE_SERVER_H_
#define CORE_SERVER_H_

#include "common/emulation_status.h"

/**
 * @brief Execute EDHOC server handshake and receive encrypted message from
 * client
 *
 * @return COM_EMULATION_SUCCESS on success, COM_EMULATION_FAILURE on failure
 */
enum com_emulation_status core_run_server(void);

#endif  // CORE_SERVER_H_