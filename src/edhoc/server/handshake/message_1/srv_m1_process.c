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

#include "edhoc/common/add_error/com_edhoc_add_cipher_suite_mismatch_error.h"
#include "edhoc/common/com_logging.h"

static bool error_code_is_suite_mismatch(const struct edhoc_context* context) {
  enum edhoc_error_code error;
  if (edhoc_error_get_code(context, &error) != EDHOC_SUCCESS) {
    error = EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;
  }
  return error == EDHOC_ERROR_CODE_WRONG_SELECTED_CIPHER_SUITE;
}

static struct srv_edhoc_message_1_process_result ok(void) {
  return (struct srv_edhoc_message_1_process_result){
      .status = SRV_EDHOC_MSG1_PROCESS_OK,
  };
}

static struct srv_edhoc_message_1_process_result internal_failure(
    const enum srv_edhoc_message_1_process_status status) {
  return (struct srv_edhoc_message_1_process_result){.status = status};
}

static struct srv_edhoc_message_1_process_result suite_mismatch_failure(
    const struct com_readonly_buffer error_buffer) {
  return (struct srv_edhoc_message_1_process_result){
      .status = SRV_EDHOC_MSG1_PROCESS_ERR_EDHOC_PROCESS,
      .error_buffer = error_buffer};
}

static struct srv_edhoc_message_1_process_result invalid_error_buffer(void) {
  return (struct srv_edhoc_message_1_process_result){
      .status = SRV_EDHOC_MSG1_PROCESS_ERR_INVALID_ERROR_BUFFER,
  };
}

static struct srv_edhoc_message_1_process_result null_context_failure(void) {
  return (struct srv_edhoc_message_1_process_result){
      .status = SRV_EDHOC_MSG1_PROCESS_ERR_NULL_CONTEXT,
  };
}

struct srv_edhoc_message_1_process_result srv_edhoc_process_message_1(
    const struct com_readonly_buffer request,
    struct edhoc_context* edhoc_context,
    const struct com_writable_buffer error_buffer) {
  if (!com_readonly_buffer_has_content(request)) {
    com_edhoc_log_error("Message 1 Process error: Empty request buffer");
    return internal_failure(SRV_EDHOC_MSG1_PROCESS_ERR_EMPTY_REQUEST_BUFFER);
  }
  if (!com_writable_buffer_is_writable(error_buffer)) {
    com_edhoc_log_error("Message 1 Process error: Invalid error buffer");
    return invalid_error_buffer();
  }

  if (edhoc_context == NULL) {
    com_edhoc_log_error("Message 1 Process error: Null context");
    return null_context_failure();
  }

  if (edhoc_message_1_process(edhoc_context, request.bytes, request.length) !=
      EDHOC_SUCCESS) {
    const struct com_readonly_buffer process_error =
        error_code_is_suite_mismatch(edhoc_context)
            ? com_edhoc_add_cipher_suite_mismatch_error(edhoc_context,
                                                        error_buffer)
            : (com_edhoc_log_error(
                   "Message 1 Process error: Processing failed"),
               (struct com_readonly_buffer){0});
    return suite_mismatch_failure(process_error);
  }
  return ok();
}

enum srv_edhoc_cleanup_context_status srv_edhoc_cleanup_context(
    struct edhoc_context* context) {
  if (context == NULL) {
    return SRV_EDHOC_CLEANUP_ERR_NULL_CONTEXT;
  }
  const int deinit_failed = edhoc_context_deinit(context) != EDHOC_SUCCESS;
  free(context);
  return deinit_failed
             ? (com_edhoc_log_error(
                    "Cleanup error: Failed to deinitialize EDHOC context"),
                SRV_EDHOC_CLEANUP_ERR_DEINIT)
             : SRV_EDHOC_CLEANUP_OK;
}