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

#include "coap/server/extract_edhoc_message/srv_coap_extract_m1.h"
#include "edhoc/server/handshake/message_1/srv_m1_process.h"
#include "edhoc/server/handshake/message_2/srv_m2_compose.h"

static struct srv_edhoc_message_1_responder_result ok(
    const struct com_readonly_buffer response) {
  return (struct srv_edhoc_message_1_responder_result){
      .status = SRV_EDHOC_MSG1_RESPONDER_OK,
      .response = response,
  };
}

static struct srv_edhoc_message_1_responder_result failure(
    const enum srv_edhoc_message_1_responder_status status,
    const struct com_readonly_buffer error_response) {
  return (struct srv_edhoc_message_1_responder_result){
      .status = status,
      .response = error_response,
  };
}

static struct srv_edhoc_message_1_responder_result invalid_response_buffer(
    void) {
  return (struct srv_edhoc_message_1_responder_result){
      .status = SRV_EDHOC_MSG1_RESPONDER_ERR_INVALID_RESPONSE_BUFFER,
  };
}

struct srv_edhoc_message_1_responder_result srv_edhoc_respond_to_message_1(
    const struct com_readonly_buffer message_1,
    struct edhoc_context* edhoc_context,
    const struct com_writable_buffer response) {
  if (!com_writable_buffer_is_writable(response)) {
    return invalid_response_buffer();
  }

  const struct srv_edhoc_message_1_process_result process_result =
      srv_edhoc_process_message_1(message_1, edhoc_context, response);
  if (process_result.status != SRV_EDHOC_MSG1_PROCESS_OK) {
    return failure(SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_1_PROCESS_FAILED,
                   process_result.error_buffer);
  }

  const struct srv_edhoc_message_2_compose_result compose_result =
      srv_edhoc_compose_message_2(edhoc_context, response);
  if (compose_result.status != SRV_EDHOC_MSG2_COMPOSE_OK) {
    return failure(SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_2_COMPOSE_FAILED,
                   (struct com_readonly_buffer){0});
  }
  return ok(compose_result.buffer);
}

const char* srv_edhoc_message_1_responder_status_code_to_string(
    const enum srv_edhoc_message_1_responder_status status) {
  switch (status) {
  case SRV_EDHOC_MSG1_RESPONDER_OK:
    return "ok";
  case SRV_EDHOC_MSG1_RESPONDER_ERR_INVALID_RESPONSE_BUFFER:
    return "invalid response buffer";
  case SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_1_PARSE_FAILED:
    return "EDHOC message 1 parse failed";
  case SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_1_PROCESS_FAILED:
    return "EDHOC message 1 process failed";
  case SRV_EDHOC_MSG1_RESPONDER_ERR_MESSAGE_2_COMPOSE_FAILED:
    return "EDHOC message 2 compose failed";
  default:
    return "unknown";
  }
}