/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Logic for responding to a Message 1 (Message 1 processing and Message
 * 2 composing)
 * @see [RFC 9528
 * 5](https://datatracker.ietf.org/doc/html/rfc9528/#name-message-formatting-and-proc)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/srv_m1_responder.h"

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/server/handshake/message_1/internal/srv_m1_responder_result_builders.h"
#include "edhoc/server/handshake/message_1/srv_m1_process.h"
#include "edhoc/server/handshake/message_1/srv_m1_process_result.h"
#include "edhoc/server/handshake/message_2/srv_m2_compose.h"

// TODO: change handlers to return the com_readonly_buffer

/**
 * WHY ARE WE CASTING TO VOID THE RETURN?
 *
 * Protocol errors are more important than internal errors, so in case something
 * bad happens, we always want to report the greater failure.
 */
struct srv_edhoc_message_1_responder_result srv_edhoc_respond_to_message_1(
    const struct srv_edhoc_message_1_request request,
    struct com_writable_buffer* response) {
  if (!com_writable_buffer_is_writable(response)) {
    return srv_edhoc_message_1_responder_failure(
        SRV_EDHOC_MSG1_RESPONDER_ERR_INVALID_RESPONSE_BUFFER);
  }

  struct srv_edhoc_message_1_process_result process_result =
      srv_edhoc_process_message_1(request, response);
  if (process_result.status != SRV_EDHOC_MSG1_PROCESS_OK) {
    return srv_edhoc_message_1_responder_failure(
        SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_1_PROCESS_FAILED);
  }
  const struct srv_edhoc_message_2_compose_result compose_result =
      srv_edhoc_compose_message_2(process_result.context, response);
  if (compose_result.status != SRV_EDHOC_MSG2_COMPOSE_OK) {
    srv_edhoc_cleanup_context(&process_result.context);
    return srv_edhoc_message_1_responder_failure(
        SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_2_COMPOSE_FAILED);
  }
  return srv_edhoc_message_1_responder_ok(process_result.context);
}

const char* srv_edhoc_handle_message_1_status_code_to_string(
    const enum srv_edhoc_message_1_responder_status status) {
  switch (status) {
  case SRV_EDHOC_MSG1_RESPONDER_OK:
    return "ok";
  case SRV_EDHOC_MSG1_RESPONDER_ERR_INVALID_RESPONSE_BUFFER:
    return "invalid response buffer";
  case SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_1_PROCESS_FAILED:
    return "EDHOC message 1 process failed";
  case SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_2_COMPOSE_FAILED:
    return "EDHOC message 2 compose failed";
  default:
    return "unknown";
  }
}