#ifndef CORE_SERVER_H_
#define CORE_SERVER_H_

#include "common/com_emulation.h"
#include "edhoc/common/com_edhoc_parameters.h"

/**
 * @brief Execute EDHOC server handshake and receive encrypted message from
 * client
 *
 * @return COM_EMULATION_SUCCESS on success, COM_EMULATION_FAILURE on failure
 */
enum com_emulation_status core_run_server(
    struct com_edhoc_parameters *edhoc_parameters);

#endif  // CORE_SERVER_H_