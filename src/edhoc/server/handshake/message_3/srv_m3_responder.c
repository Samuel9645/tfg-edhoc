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

#include "edhoc/server/handshake/message_3/srv_m3_parser.h"
#include "edhoc/server/handshake/message_3/srv_m3_process.h"
#include "edhoc/server/handshake/message_4/srv_m4_compose.h"

static struct srv_edhoc_message_3_responder_result ok(
    const struct com_readonly_buffer response) {
  return (struct srv_edhoc_message_3_responder_result){
      .status = SRV_EDHOC_MSG3_RESPONDER_OK,
      .response = response,
  };
}

static struct srv_edhoc_message_3_responder_result failure(
    const enum srv_edhoc_message_3_responder_status status,
    const struct com_readonly_buffer error_response) {
  return (struct srv_edhoc_message_3_responder_result){
      .status = status,
      .response = error_response,
  };
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

  const struct srv_edhoc_parse_message_3_result parse_result =
      srv_edhoc_parse_message_3(request.raw_payload, request.edhoc_context,
                                response);
  if (parse_result.status != SRV_EDHOC_MSG3_PARSE_OK) {
    return failure(SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_3_PARSE,
                   parse_result.buffer);
  }

  const struct srv_edhoc_message_3_request process_request = {
      .parsed_message_3 = parse_result.buffer,
      .edhoc_context = request.edhoc_context,
  };
  const struct srv_edhoc_message_3_process_result process_result =
      srv_edhoc_process_message_3(process_request, response);
  if (process_result.status != SRV_EDHOC_MSG3_PROCESS_OK) {
    return failure(SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_3_PROCESS,
                   process_result.error_buffer);
  }

  const struct srv_edhoc_message_4_compose_result compose_result =
      srv_edhoc_compose_message_4(request.edhoc_context, response);
  if (compose_result.status != SRV_EDHOC_MSG4_COMPOSE_OK) {
    return failure(SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_4_COMPOSE,
                   compose_result.buffer);
  }
  return ok(compose_result.buffer);
}

const char* srv_edhoc_handle_message_3_status_code_to_string(
    const enum srv_edhoc_message_3_responder_status status) {
  switch (status) {
  case SRV_EDHOC_MSG3_RESPONDER_OK:
    return "ok";
  case SRV_EDHOC_MSG3_RESPONDER_ERR_INVALID_RESPONSE_BUFFER:
    return "invalid response buffer";
  case SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_3_PARSE:
    return "message 3 parse failed";
  case SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_3_PROCESS:
    return "message 3 process failed";
  case SRV_EDHOC_MSG3_RESPONDER_ERR_MESSAGE_4_COMPOSE:
    return "message 4 compose failed";
  default:
    return "unknown";
  }
}
