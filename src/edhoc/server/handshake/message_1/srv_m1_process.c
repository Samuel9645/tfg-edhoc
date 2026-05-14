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
#include "edhoc/common/com_edhoc_setup_context.h"

static bool error_code_is_suite_mismatch(const struct edhoc_context* context) {
  enum edhoc_error_code error;
  if (edhoc_error_get_code(context, &error) != EDHOC_SUCCESS) {
    error = EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;
  }
  return error == EDHOC_ERROR_CODE_WRONG_SELECTED_CIPHER_SUITE;
}

enum { SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE = 8 };

static struct com_readonly_buffer add_cipher_suite_mismatch_error(
    const struct edhoc_context* context,
    const struct com_writable_buffer response_data) {
  int32_t peer_suites[SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE] = {0};
  size_t peer_length = 0;
  int32_t own_suites[SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE] = {0};
  size_t own_length = 0;
  if (edhoc_error_get_cipher_suites(
          context, own_suites, SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE, &own_length,
          peer_suites, SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE,
          &peer_length) != EDHOC_SUCCESS) {
    return com_edhoc_add_protocol_error_with_description_view(
        context,
        "Message 1 Process error: Fatal: wrong cipher suite error but failed "
        "to get cipher suites",
        response_data);
  }
  const struct edhoc_error_info error_info = {
      .cipher_suites = own_suites,
      .total_entries = SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE,
      .written_entries = own_length,
  };
  return com_edhoc_add_protocol_error_view(context, &error_info, response_data);
}

static struct srv_edhoc_message_1_process_result ok(
    struct edhoc_context* context) {
  return (struct srv_edhoc_message_1_process_result){
      .status = SRV_EDHOC_MSG1_PROCESS_OK,
      .context = context,
  };
}

static struct srv_edhoc_message_1_process_result failure(
    const enum srv_edhoc_message_1_process_status status,
    const struct com_readonly_buffer error_buffer) {
  return (struct srv_edhoc_message_1_process_result){
      .status = status,
      .error_buffer = error_buffer,
  };
}

static struct srv_edhoc_message_1_process_result invalid_error_buffer(void) {
  return (struct srv_edhoc_message_1_process_result){
      .status = SRV_EDHOC_MSG1_PROCESS_ERR_INVALID_ERROR_BUFFER,
  };
}

struct srv_edhoc_message_1_process_result srv_edhoc_process_message_1(
    const struct srv_edhoc_message_1_request request,
    const struct com_writable_buffer error_buffer,
    const struct com_edhoc_parameters edhoc_parameters) {
  if (!com_writable_buffer_is_writable(error_buffer)) {
    return invalid_error_buffer();
  }
  if (!com_readonly_buffer_has_content(request.payload)) {
    return failure(
        SRV_EDHOC_MSG1_PROCESS_ERR_EMPTY_REQUEST_BUFFER,
        com_edhoc_add_internal_error_view(
            "Message 1 Process error: Empty request buffer", error_buffer));
  }

  struct edhoc_context* context = calloc(1, sizeof(struct edhoc_context));
  if (context == NULL) {
    return failure(
        SRV_EDHOC_MSG1_PROCESS_ERR_CALLOC,
        com_edhoc_add_internal_error_view(
            "Message 1 Process error: Context calloc failed", error_buffer));
  }

  const struct com_edhoc_setup_context_result setup_context_result =
      com_edhoc_setup_context(context, edhoc_parameters, error_buffer);
  if (setup_context_result.status != COM_EDHOC_SETUP_CTX_OK) {
    const struct srv_edhoc_message_1_process_result failure_result =
        failure(SRV_EDHOC_MSG1_PROCESS_ERR_EDHOC_CONTEXT_SETUP,
                setup_context_result.error_buffer);
    free(context);
    return failure_result;
  }

  if (edhoc_message_1_process(context, request.payload.bytes,
                              request.payload.length) != EDHOC_SUCCESS) {
    const struct com_readonly_buffer process_error =
        error_code_is_suite_mismatch(context)
            ? add_cipher_suite_mismatch_error(context, error_buffer)
            : com_edhoc_add_protocol_error_with_description_view(
                  context, "Message 1 Process error: Processing failed",
                  error_buffer);
    const struct srv_edhoc_message_1_process_result failure_result =
        failure(SRV_EDHOC_MSG1_PROCESS_ERR_EDHOC_PROCESS, process_error);
    srv_edhoc_cleanup_context(context);
    context = NULL;
    return failure_result;
  }
  return ok(context);
}

enum srv_edhoc_cleanup_context_status srv_edhoc_cleanup_context(
    struct edhoc_context* context) {
  if (context == NULL) {
    return SRV_EDHOC_CLEANUP_ERR_NULL_CONTEXT;
  }
  if (edhoc_context_deinit(context) != EDHOC_SUCCESS) {
    return SRV_EDHOC_CLEANUP_ERR_DEINIT;
  }
  free(context);
  context = NULL;
  return SRV_EDHOC_CLEANUP_OK;
}