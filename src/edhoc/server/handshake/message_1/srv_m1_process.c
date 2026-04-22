/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 1 processing on the responder side
 * @see [RFC
 * 9528 5.2.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-responder-processing-of-mes)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/srv_m1_process.h"

#include <stdlib.h>

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"
#include "edhoc/common/com_edhoc_context_setup.h"
#include "edhoc/server/handshake/message_1/internal/srv_m1_process_result_builders.h"

static struct com_readonly_buffer add_internal_error_to_buffer(
    const char* error_message, struct com_writable_buffer* buffer) {
  return com_edhoc_add_internal_error_result(error_message, buffer).buffer;
}

struct srv_edhoc_message_1_process_result srv_edhoc_process_message_1(
    const struct srv_edhoc_message_1_request request,
    struct com_writable_buffer* error_buffer) {
  if (!com_writable_buffer_is_writable(error_buffer)) {
    return srv_edhoc_message_1_process_invalid_error_buffer_failure();
  }
  if (request.credentials == NULL) {
    return srv_edhoc_message_1_process_failure(
        SRV_EDHOC_MSG1_PROCESS_ERR_NULL_CREDENTIALS,
        add_internal_error_to_buffer("Null credentials", error_buffer));
  }
  if (!com_readonly_buffer_has_content(request.payload)) {
    return srv_edhoc_message_1_process_failure(
        SRV_EDHOC_MSG1_PROCESS_ERR_INVALID_REQUEST_BUFFER,
        add_internal_error_to_buffer("Invalid request buffer", error_buffer));
  }

  struct edhoc_context* context = calloc(1, sizeof(struct edhoc_context));
  if (context == NULL) {
    return srv_edhoc_message_1_process_failure(
        SRV_EDHOC_MSG1_PROCESS_ERR_CALLOC,
        add_internal_error_to_buffer("Context calloc failed", error_buffer));
  }
  if (com_edhoc_setup_context(context, request.credentials) !=
      COM_EDHOC_SETUP_CTX_OK) {
    free(context);
    return srv_edhoc_message_1_process_failure(
        SRV_EDHOC_MSG1_PROCESS_ERR_EDHOC_CONTEXT_SETUP,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Context setup failed", error_buffer));
  }
  if (edhoc_message_1_process(context, request.payload.bytes,
                              request.payload.length) != EDHOC_SUCCESS) {
    srv_edhoc_cleanup_context(&context);
    return srv_edhoc_message_1_process_failure(
        SRV_EDHOC_MSG1_PROCESS_ERR_EDHOC_PROCESS,
        com_edhoc_add_protocol_error_with_description_view(
            context, "Message 1 processing failed", error_buffer));
  }
  return srv_edhoc_message_1_process_ok(context);
}

enum srv_edhoc_cleanup_context_status srv_edhoc_cleanup_context(
    struct edhoc_context** context) {
  if (*context == NULL) {
    return SRV_EDHOC_CLEANUP_ERR_NULL_CONTEXT;
  }
  if (edhoc_context_deinit(*context) != EDHOC_SUCCESS) {
    return SRV_EDHOC_CLEANUP_ERR_DEINIT;
  }
  free(*context);
  *context = NULL;
  return SRV_EDHOC_CLEANUP_OK;
}