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
#include "edhoc/server/handshake/message_4/internal/srv_m4_compose_result_builders.h"

static struct com_readonly_buffer add_internal_error_to_buffer(
    const char* error_message, struct com_writable_buffer* buffer) {
  return com_edhoc_add_internal_error(error_message, buffer).buffer;
}

static struct com_readonly_buffer add_compose_error_to_buffer(
    const struct edhoc_context* context, struct com_writable_buffer* buffer) {
  return com_edhoc_add_protocol_error_with_description(
             context, "Message 4 composition failed", buffer)
      .buffer;
}

struct srv_edhoc_message_4_compose_result srv_edhoc_compose_message_4(
    struct edhoc_context* context, struct com_writable_buffer* compose_buffer) {
  if (!com_writable_buffer_is_writable(compose_buffer)) {
    return srv_edhoc_message_4_compose_invalid_compose_buffer_failure();
  }
  if (context == NULL) {
    return srv_edhoc_message_4_compose_failure(
        SRV_EDHOC_MSG4_COMPOSE_ERR_NULL_CONTEXT,
        add_internal_error_to_buffer("Null EDHOC context", compose_buffer));
  }

  if (edhoc_message_4_compose(context, compose_buffer->bytes,
                              compose_buffer->capacity,
                              &compose_buffer->length) != EDHOC_SUCCESS) {
    return srv_edhoc_message_4_compose_failure(
        SRV_EDHOC_MSG4_COMPOSE_ERR_COMPOSE_FAILED,
        add_compose_error_to_buffer(context, compose_buffer));
  }
  if (!com_writable_buffer_has_content(compose_buffer)) {
    return srv_edhoc_message_4_compose_failure(
        SRV_EDHOC_MSG4_COMPOSE_ERR_EMPTY_COMPOSE,
        add_internal_error_to_buffer("Empty compose result", compose_buffer));
  }
  const struct com_readonly_conversion_result conversion_result =
      com_writable_as_readonly(compose_buffer);
  if (conversion_result.status != COM_RDONLY_CONV_OK) {
    // SHOULD NEVER HAPPEN
    return srv_edhoc_message_4_compose_failure(
        SRV_EDHOC_MSG4_COMPOSE_ERR_BUFFER_CONVERSION,
        add_internal_error_to_buffer("Invalid compose buffer", compose_buffer));
  }
  return srv_edhoc_message_4_compose_ok(conversion_result.buffer);
}
