/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 1 specific EDHOC error-response composition.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/srv_m1_errors.h"

#include <edhoc_helpers.h>

#include "edhoc/server/handshake/common/handle_libedhoc_errors.h"

enum { EDH_SRV_CIPHER_SUITES_ARRAY_SIZE = 8 };

/**
 * WHY DO WE NEED THIS?
 * The edhoc_error_get_cipher_suites function requires a buffer to write the
 * cipher suites into, if we don't store this buffer in this struct the pointer
 * will not be valid after the prepare_message_1_error_context function returns
 */
// ReSharper disable once CppClassNeverUsed it is acctually used in the
// test_srv_message_1_errors.c test
struct edh_srv_message_1_error_context {
  struct edhoc_error_info info;
  int32_t suites_buffer[EDH_SRV_CIPHER_SUITES_ARRAY_SIZE];
};

static void prepare_message_1_error_context(
    const int edhoc_api_result, const struct edhoc_context* context,
    const char* generic_error_message,
    struct edh_srv_message_1_error_context* error_ctx) {
  if (edhoc_api_result != EDHOC_ERROR_CODE_WRONG_SELECTED_CIPHER_SUITE ||
      context == NULL) {
    edh_srv_set_error_info(generic_error_message, &error_ctx->info);
    return;
  }

  int32_t peer_suites[EDH_SRV_CIPHER_SUITES_ARRAY_SIZE] = {0};
  size_t peer_len = 0;
  size_t own_len = 0;
  /*
   TODO: verify this:
   The variable peer_suites is allocated but its output
   parameter peer_len is never used after the call to
   edhoc_error_get_cipher_suites. Consider removing this unused variable and
   its associated parameter, or document why the peer suites are retrieved but
   not utilized.
   */
  edhoc_error_get_cipher_suites(
      context, error_ctx->suites_buffer, EDH_SRV_CIPHER_SUITES_ARRAY_SIZE,
      &own_len, peer_suites, ARRAY_SIZE(peer_suites), &peer_len);

  edh_srv_set_error_info(generic_error_message, &error_ctx->info);
}

void edh_srv_message_1_handler_add_error(
    const int edhoc_api_result, const struct edhoc_context* edhoc_context,
    const char* generic_error_message,
    struct com_writable_buffer* response_data) {
  struct edh_srv_message_1_error_context error_ctx = {0};
  prepare_message_1_error_context(edhoc_api_result, edhoc_context,
                                  generic_error_message, &error_ctx);
  edh_srv_add_edhoc_error_to_response(edhoc_api_result, &error_ctx.info,
                                      response_data);
}
