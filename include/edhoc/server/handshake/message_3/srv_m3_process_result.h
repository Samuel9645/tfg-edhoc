/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Results of Message 3 processing on the responder side.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_PROCESS_RESULT_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_PROCESS_RESULT_H_

#include "common/com_data_models.h"

enum srv_edhoc_message_3_process_status {
  SRV_EDHOC_MSG3_PROCESS_OK = 0,
  SRV_EDHOC_MSG3_PROCESS_ERR_INVALID_ERROR_BUFFER,
  SRV_EDHOC_MSG3_PROCESS_ERR_NULL_EDHOC_CONTEXT,
  SRV_EDHOC_MSG3_PROCESS_ERR_EMPTY_PARSED_MESSAGE_3,
  SRV_EDHOC_MSG3_PROCESS_ERR_EDHOC_MESSAGE_3_PROCESS_FAILED,
};

struct srv_edhoc_message_3_process_result {
  const enum srv_edhoc_message_3_process_status status;
  const struct com_readonly_buffer error_buffer;
};

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_SRV_M3_PROCESS_RESULT_H_
