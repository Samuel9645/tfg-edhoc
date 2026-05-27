/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Message 3 processing on the responder side.
 * @see [RFC
 * 9528 5.4.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-processing-of-mess)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_3/srv_m3_process.h"

#include <edhoc.h>

#include "../../../../../include/common/com_logging.h"

static struct srv_edhoc_message_3_process_result ok(void) {
  return (struct srv_edhoc_message_3_process_result){
      .status = SRV_EDHOC_MSG3_PROCESS_OK,
  };
}

static struct srv_edhoc_message_3_process_result failure(
    const enum srv_edhoc_message_3_process_status status) {
  return (struct srv_edhoc_message_3_process_result){.status = status};
}

struct srv_edhoc_message_3_process_result srv_edhoc_process_message_3(
    const struct srv_edhoc_message_3_request request) {
  if (request.edhoc_context == NULL) {
    com_log_error("Message 3 Process error: Null context");
    return failure(SRV_EDHOC_MSG3_PROCESS_ERR_NULL_EDHOC_CONTEXT);
  }
  if (!com_readonly_buffer_has_content(request.message_3)) {
    com_log_error("Message 3 Process error: Empty parsed message");
    return failure(SRV_EDHOC_MSG3_PROCESS_ERR_EMPTY_PARSED_MESSAGE_3);
  }

  if (edhoc_message_3_process(request.edhoc_context, request.message_3.bytes,
                              request.message_3.length) != EDHOC_SUCCESS) {
    com_log_error("Message 3 Process error: Processing failed");
    return failure(SRV_EDHOC_MSG3_PROCESS_ERR_EDHOC_MESSAGE_3_PROCESS_FAILED);
  }
  return ok();
}
