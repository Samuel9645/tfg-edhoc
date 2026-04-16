/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Results of Message 1 processing on the responder side (including the
 * created EDHOC context)
 * @see [RFC
 * 9528 5.2.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-processing-of-mes)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PROCESS_RESULT_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PROCESS_RESULT_H_
#include "common/com_data_models.h"

enum srv_edhoc_message_1_process_status {
  SRV_EDHOC_MSG1_PROCESS_OK = 0,
  SRV_EDHOC_MSG1_PROCESS_ERR_INVALID_ERROR_BUFFER,
  SRV_EDHOC_MSG1_PROCESS_ERR_NULL_CREDENTIALS,
  SRV_EDHOC_MSG1_PROCESS_ERR_INVALID_REQUEST_BUFFER,
  SRV_EDHOC_MSG1_PROCESS_ERR_CALLOC_FAILED,
  SRV_EDHOC_MSG1_PROCESS_ERR_EDHOC_CONTEXT_SETUP_FAILED,
  SRV_EDHOC_MSG1_PROCESS_ERR_EDHOC_MESSAGE_1_PROCESS_FAILED
};

struct srv_edhoc_message_1_process_result {
  enum srv_edhoc_message_1_process_status status;
  /**
   * Pointer to the allocated EDHOC context on success, NULL on failure.
   * @warning The caller is responsible for freeing the allocated context using
   * edhoc_context_deinit() to clean up the library context and then free() when
   * no longer needed.
   */
  struct edhoc_context* context;
  struct com_readonly_buffer error_buffer;
};

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_SRV_M1_PROCESS_RESULT_H_