/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 22/04/2026
 * @brief Logic for responding to Message 2 (process Message 2 and compose
 * Message 3)
 * @see [RFC 9528
 * A.2.1](https://datatracker.ietf.org/doc/html/rfc9528/#name-the-forward-message-flow)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/handshake/message_2/cli_m2_responder.h"

#include "edhoc/client/handshake/message_2/cli_m2_process.h"
#include "edhoc/client/handshake/message_3/cli_m3_compose.h"

static struct cli_edhoc_message_2_responder_result invalid_response_failure(
    void) {
  return (struct cli_edhoc_message_2_responder_result){
      .status = CLI_EDHOC_MSG2_INITIATOR_ERR_INVALID_RESPONSE_BUFFER,
  };
}

static struct cli_edhoc_message_2_responder_result failure(
    const enum cli_edhoc_message_2_initiator_status status,
    const struct com_readonly_buffer request) {
  return (struct cli_edhoc_message_2_responder_result){
      .status = status,
      .buffer = request,
  };
}

static struct cli_edhoc_message_2_responder_result ok(
    const struct com_readonly_buffer request) {
  return (struct cli_edhoc_message_2_responder_result){
      .status = CLI_EDHOC_MSG2_INITIATOR_OK,
      .buffer = request,
  };
}

struct cli_edhoc_message_2_responder_result cli_edhoc_respond_to_message_2(
    const struct cli_edhoc_message_2_initiator_request request,
    const struct com_writable_buffer response_buffer) {
  if (!com_writable_buffer_is_writable(response_buffer)) {
    return invalid_response_failure();
  }

  const struct cli_edhoc_message_2_process_result process_result =
      cli_edhoc_process_message_2(request.edhoc_context, request.message_2);
  if (process_result.status != CLI_EDHOC_MSG2_PROCESS_OK) {
    return failure(CLI_EDHOC_MSG2_INITIATOR_ERR_MESSAGE_2_PROCESS,
                   (struct com_readonly_buffer){0});
  }

  const struct cli_edhoc_message_3_compose_result compose_result =
      cli_edhoc_compose_message_3(request.edhoc_context, response_buffer);
  if (compose_result.status != CLI_EDHOC_MSG3_COMPOSE_OK) {
    return failure(CLI_EDHOC_MSG2_INITIATOR_ERR_MESSAGE_3_COMPOSE,
                   (struct com_readonly_buffer){0});
  }

  return ok(compose_result.buffer);
}

const char* cli_edhoc_respond_to_message_2_status_code_to_string(
    const enum cli_edhoc_message_2_initiator_status status) {
  switch (status) {
  case CLI_EDHOC_MSG2_INITIATOR_OK:
    return "ok";
  case CLI_EDHOC_MSG2_INITIATOR_ERR_INVALID_RESPONSE_BUFFER:
    return "invalid request buffer";
  case CLI_EDHOC_MSG2_INITIATOR_ERR_MESSAGE_2_PROCESS:
    return "message 2 process failed";
  case CLI_EDHOC_MSG2_INITIATOR_ERR_MESSAGE_3_COMPOSE:
    return "message 3 compose failed";
  default:
    return "unknown";
  }
}
