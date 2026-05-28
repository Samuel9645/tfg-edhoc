/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 2 composition on the responder side
 * @see [RFC
 * 9528 5.3.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-composition-of-me)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_2/srv_m2_compose.h"

#include "common/com_logging.h"

static struct srv_edhoc_message_2_compose_result ok(
    const struct com_readonly_buffer buffer) {
  return (struct srv_edhoc_message_2_compose_result){
      .status = SRV_EDHOC_MSG2_COMPOSE_OK,
      .buffer = buffer,
  };
}

static struct srv_edhoc_message_2_compose_result failure(
    const enum srv_edhoc_message_2_compose_status status) {
  return (struct srv_edhoc_message_2_compose_result){.status = status};
}

static struct srv_edhoc_message_2_compose_result invalid_compose_buffer(void) {
  return (struct srv_edhoc_message_2_compose_result){
      .status = SRV_EDHOC_MSG2_COMPOSE_ERR_INVALID_COMPOSE_BUFFER,
  };
}

struct srv_edhoc_message_2_compose_result srv_edhoc_compose_message_2(
    struct edhoc_context* context,
    const struct com_writable_buffer compose_buffer) {
  if (!com_writable_buffer_is_writable(compose_buffer)) {
    return invalid_compose_buffer();
  }
  if (context == NULL) {
    com_log_error("Message 2 Compose error: Null context");
    return failure(SRV_EDHOC_MSG2_COMPOSE_ERR_NULL_CONTEXT);
  }

  size_t written_length = 0;
  if (edhoc_message_2_compose(context, compose_buffer.bytes,
                              compose_buffer.capacity,
                              &written_length) != EDHOC_SUCCESS) {
    com_log_error("Message 2 Compose error: Composition failed");
    return failure(SRV_EDHOC_MSG2_COMPOSE_ERR_COMPOSE);
  }
  const struct com_readonly_conversion_result conversion_result =
      com_writable_as_readonly(compose_buffer, written_length);
  if (conversion_result.status != COM_RDONLY_CONV_OK) {
    com_log_error("Message 2 Compose error: Empty compose result");
    return failure(SRV_EDHOC_MSG2_COMPOSE_ERR_EMPTY_COMPOSE);
  }
  return ok(conversion_result.buffer);
}