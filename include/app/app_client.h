#ifndef CORE_CLIENT_H_
#define CORE_CLIENT_H_

#include "common/com_emulation.h"
#include "edhoc/common/com_edhoc_parameters.h"

/**
 * @brief Executes EDHOC client handshake and send encrypted message to server
 * @param edhoc_parameters Parameters to use in the EDHOC handshake
 * @param preferred_suites Array of preferred suites to use
 * @return COM_EMULATION_SUCCESS on success, COM_EMULATION_FAILURE on failure
 */
enum com_emulation_status core_run_client(
    struct com_edhoc_parameters edhoc_parameters,
    struct com_edhoc_cipher_suite_list preferred_suites);

#endif  // CORE_CLIENT_H_