/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Result codes for client handshake Message 2 processing.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_RESULT_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_RESULT_H_

#include "common/data_models.h"

enum edh_cli_message_2_process_status {
  EDH_CLI_MSG2_PROCESS_OK = 0,
  EDH_CLI_MSG2_PROCESS_ERR_INVALID_ARGS,
  EDH_CLI_MSG2_PROCESS_ERR_EDHOC_MESSAGE_2_PROCESS_FAILED,
};

struct edh_cli_message_2_result {
  enum edh_cli_message_2_process_status status;
  struct com_writable_buffer output;
};

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_RESULT_H_
