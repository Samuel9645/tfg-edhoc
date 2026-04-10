/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Result codes for client handshake Message 2 processing.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_RESULT_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_RESULT_H_

#include "common/com_data_models.h"

enum cli_edhoc_message_2_process_status {
  CLI_EDHOC_MSG2_PROCESS_OK = 0,
  CLI_EDHOC_MSG2_PROCESS_ERR_INVALID_ARGS,
  CLI_EDHOC_MSG2_PROCESS_ERR_EDHOC_MESSAGE_2_PROCESS_FAILED,
};

struct cli_edhoc_message_2_process_result {
  enum cli_edhoc_message_2_process_status status;
  struct com_writable_buffer output;
};

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_RESULT_H_
