/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Logic for responding to a Message 3 (Message 3 processing and Message
 * 4 composing)
 * @see [RFC 9528
 * 5](https://datatracker.ietf.org/doc/html/rfc9528/#name-message-formatting-and-proc)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_3/srv_m3_responder.h"

#include "edhoc/server/handshake/message_3/srv_m3_process.h"

static struct srv_edhoc_message_3_responder_result ok(
    const struct com_readonly_buffer response) {
  return (struct srv_edhoc_message_3_responder_result){
      .status = SRV_EDHOC_MSG3_RESPONDER_OK,
      .response = response,
  };
}

static struct srv_edhoc_message_3_responder_result process_failure(void) {
  return (struct srv_edhoc_message_3_responder_result){
      .status = SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_3_PROCESS};
}

static struct srv_edhoc_message_3_responder_result invalid_response_buffer(
    void) {
  return (struct srv_edhoc_message_3_responder_result){
      .status = SRV_EDHOC_MSG3_RESPONDER_ERR_INVALID_RESPONSE_BUFFER,
  };
}

struct srv_edhoc_message_3_responder_result srv_edhoc_respond_to_message_3(
    const struct srv_edhoc_message_3_responder_request request,
    const struct com_writable_buffer response) {
  if (!com_writable_buffer_is_writable(response)) {
    return invalid_response_buffer();
  }

  const struct srv_edhoc_message_3_request process_request = {
      .message_3 = request.message_3,
      .edhoc_context = request.edhoc_context,
  };
  const struct srv_edhoc_message_3_process_result process_result =
      srv_edhoc_process_message_3(process_request);
  if (process_result.status != SRV_EDHOC_MSG3_PROCESS_OK) {
    return process_failure();
  }
  return ok((struct com_readonly_buffer){.bytes = response.bytes, .length = 0});
}

const char* srv_edhoc_handle_message_3_status_code_to_string(
    const enum srv_edhoc_message_3_responder_status status) {
  switch (status) {
  case SRV_EDHOC_MSG3_RESPONDER_OK:
    return "ok";
  case SRV_EDHOC_MSG3_RESPONDER_ERR_INVALID_RESPONSE_BUFFER:
    return "invalid response buffer";
  case SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_3_PROCESS:
    return "message 3 process failed";
  default:
    return "unknown";
  }
}
