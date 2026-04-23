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

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

static struct srv_edhoc_message_2_compose_result ok(
    const struct com_readonly_buffer buffer) {
  return (struct srv_edhoc_message_2_compose_result){
      .status = SRV_EDHOC_MSG2_COMPOSE_OK,
      .buffer = buffer,
  };
}

static struct srv_edhoc_message_2_compose_result failure(
    const enum srv_edhoc_message_2_compose_status status,
    const struct com_readonly_buffer error) {
  return (struct srv_edhoc_message_2_compose_result){
      .status = status,
      .buffer = error,
  };
}

static struct srv_edhoc_message_2_compose_result invalid_compose_buffer(void) {
  return (struct srv_edhoc_message_2_compose_result){
      .status = SRV_EDHOC_MSG2_COMPOSE_ERR_INVALID_COMPOSE_BUFFER,
  };
}

static struct com_readonly_buffer add_internal_error_to_buffer(
    const char* error_message, struct com_writable_buffer* buffer) {
  return com_edhoc_add_internal_error_result(error_message, buffer).buffer;
}

static struct com_readonly_buffer add_compose_error_to_buffer(
    const struct edhoc_context* context, struct com_writable_buffer* buffer) {
  return com_edhoc_add_protocol_error_with_description_view(
      context, "Message 2 composition failed", buffer);
}

struct srv_edhoc_message_2_compose_result srv_edhoc_compose_message_2(
    struct edhoc_context* context, struct com_writable_buffer* compose_buffer) {
  if (!com_writable_buffer_is_writable(compose_buffer)) {
    return invalid_compose_buffer();
  }
  if (context == NULL) {
    return failure(
        SRV_EDHOC_MSG2_COMPOSE_ERR_NULL_CONTEXT,
        add_internal_error_to_buffer("Null EDHOC context", compose_buffer));
  }

  if (edhoc_message_2_compose(context, compose_buffer->bytes,
                              compose_buffer->capacity,
                              &compose_buffer->length) != EDHOC_SUCCESS) {
    return failure(SRV_EDHOC_MSG2_COMPOSE_ERR_COMPOSE,
                   add_compose_error_to_buffer(context, compose_buffer));
  }
  if (!com_writable_buffer_has_content(compose_buffer)) {
    return failure(
        SRV_EDHOC_MSG2_COMPOSE_ERR_EMPTY_COMPOSE,
        add_internal_error_to_buffer("Empty compose result", compose_buffer));
  }
  const struct com_readonly_conversion_result conversion_result =
      com_writable_as_readonly(compose_buffer);
  if (conversion_result.status != COM_RDONLY_CONV_OK) {
    // SHOULD NEVER HAPPEN
    return failure(
        SRV_EDHOC_MSG2_COMPOSE_ERR_BUFFER_CONVERSION,
        add_internal_error_to_buffer("Invalid compose buffer", compose_buffer));
  }
  return ok(conversion_result.buffer);
}