/**
 * @file errors.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 3 specific EDHOC error-response composition.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_3/errors.h"

#include "edhoc/server/handshake/common/handle_libedhoc_errors.h"

typedef struct {
  struct edhoc_error_info info;
} edhs_m3_error_context_t;

static edhs_m3_error_context_t prepare_message_3_error_context(
    const char* generic_error_message) {
  edhs_m3_error_context_t error_ctx = {0};
  edh_srv_set_error_info(generic_error_message, &error_ctx.info);
  return error_ctx;
}

void edh_message_3_handler_add_error(const int edhoc_api_result, const char* generic_error_message,
    com_response_buffer_t* response_data) {
  const edhs_m3_error_context_t error_ctx =
      prepare_message_3_error_context(generic_error_message);

  edh_srv_add_edhoc_error_to_response(edhoc_api_result, &error_ctx.info,
                                      response_data);
}
