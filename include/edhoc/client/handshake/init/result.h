/**
 * @file result.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Result codes for client handshake initialization step.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_INIT_RESULT_H_
#define EDHOC_CLIENT_HANDSHAKE_INIT_RESULT_H_

/**
 * @brief Status codes for client handshake initialization.
 */
typedef enum edh_cli_init_status {
  EDH_CLI_INIT_OK = 0,
  EDH_CLI_INIT_ERR_INVALID_ARGS,
  EDH_CLI_INIT_ERR_EDHOC_CONTEXT_SETUP_FAILED,
} edh_cli_init_status_t;

#endif  // EDHOC_CLIENT_HANDSHAKE_INIT_RESULT_H_
