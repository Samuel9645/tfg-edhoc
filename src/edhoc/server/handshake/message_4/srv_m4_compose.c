/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Message 4 compose on the responder side.
 * @see [RFC
 * 9528 5.5.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-composition-of-mes)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_4/srv_m4_compose.h"

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

static struct srv_edhoc_message_4_compose_result ok(
    const struct com_readonly_buffer buffer) {
  return (struct srv_edhoc_message_4_compose_result){
      .status = SRV_EDHOC_MSG4_COMPOSE_OK,
      .buffer = buffer,
  };
}

static struct srv_edhoc_message_4_compose_result failure(
    const enum srv_edhoc_message_4_compose_status status,
    const struct com_readonly_buffer error) {
  return (struct srv_edhoc_message_4_compose_result){
      .status = status,
      .buffer = error,
  };
}

static struct srv_edhoc_message_4_compose_result invalid_buffer(void) {
  return (struct srv_edhoc_message_4_compose_result){
      .status = SRV_EDHOC_MSG4_COMPOSE_ERR_INVALID_COMPOSE_BUFFER,
  };
}

static struct com_readonly_buffer add_compose_error_to_buffer(
    const struct edhoc_context* context,
    const struct com_writable_buffer buffer) {
  return com_edhoc_add_protocol_error_with_description_view(
      context, "Message 4 Compose error: Composition failed", buffer);
}

struct srv_edhoc_message_4_compose_result srv_edhoc_compose_message_4(
    struct edhoc_context* context,
    const struct com_writable_buffer compose_buffer) {
  if (!com_writable_buffer_is_writable(compose_buffer)) {
    return invalid_buffer();
  }
  if (context == NULL) {
    return failure(
        SRV_EDHOC_MSG4_COMPOSE_ERR_NULL_CONTEXT,
        com_edhoc_add_internal_error_view(
            "Message 4 Compose error: Null context", compose_buffer));
  }

  size_t written_length = 0;
  if (edhoc_message_4_compose(context, compose_buffer.bytes,
                              compose_buffer.capacity,
                              &written_length) != EDHOC_SUCCESS) {
    return failure(SRV_EDHOC_MSG4_COMPOSE_ERR_COMPOSE_FAILED,
                   add_compose_error_to_buffer(context, compose_buffer));
  }
  const struct com_readonly_conversion_result conversion_result =
      com_writable_as_readonly(compose_buffer, written_length);
  if (conversion_result.status != COM_RDONLY_CONV_OK) {
    return failure(
        SRV_EDHOC_MSG4_COMPOSE_ERR_EMPTY_COMPOSE,
        com_edhoc_add_internal_error_view(
            "Message 4 Compose error: Empty compose result", compose_buffer));
  }
  return ok(conversion_result.buffer);
}
