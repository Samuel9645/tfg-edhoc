/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 1 specific EDHOC error-response composition.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/srv_m1_errors.h"

#include <edhoc_helpers.h>

#include "edhoc/common/add_edhoc_error_info.h"

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

enum edh_srv_prep_m1_error_context_status {
  EDH_SRV_PREP_M1_ERROR_CTX_OK = 0,
  EDH_SRV_PREP_M1_ERROR_CTX_ERR_SET_INFO,
  EDH_SRV_PREP_M1_ERROR_CTX_ERR_GET_SUITES
};

static enum edh_srv_prep_m1_error_context_status
prepare_message_1_error_context(
    const int edhoc_api_result, const struct edhoc_context* context,
    const char* generic_error_message,
    struct edh_srv_message_1_error_context* error_ctx) {
  if (edh_com_set_error_info(generic_error_message, &error_ctx->info) !=
      EDH_COM_SET_ERROR_INFO_OK) {
    return EDH_SRV_PREP_M1_ERROR_CTX_ERR_SET_INFO;
  }
  if (edhoc_api_result != EDHOC_ERROR_CODE_WRONG_SELECTED_CIPHER_SUITE ||
      context == NULL) {
    return EDH_SRV_PREP_M1_ERROR_CTX_OK;
  }
  int32_t peer_suites[EDH_SRV_CIPHER_SUITES_ARRAY_SIZE] = {0};
  size_t peer_len = 0;
  size_t own_len = 0;
  /**
   * TODO: verify this:
   * The variable peer_suites is allocated but its output
   * parameter peer_len is never used after the call to
   * edhoc_error_get_cipher_suites. Consider removing this unused variable and
   * its associated parameter, or document why the peer suites are retrieved but
   * not utilized.
   *
   * - libedhoc requires peer suites to be valid even though they are not used
   */
  if (edhoc_error_get_cipher_suites(context, error_ctx->suites_buffer,
                                    EDH_SRV_CIPHER_SUITES_ARRAY_SIZE, &own_len,
                                    peer_suites, ARRAY_SIZE(peer_suites),
                                    &peer_len) != EDHOC_SUCCESS) {
    return EDH_SRV_PREP_M1_ERROR_CTX_ERR_GET_SUITES;
  }
  return EDH_SRV_PREP_M1_ERROR_CTX_OK;
}

enum edh_srv_message_1_handler_add_error_status
edh_srv_message_1_handler_add_error(const int edhoc_api_result,
                                    const struct edhoc_context* edhoc_context,
                                    const char* generic_error_message,
                                    struct com_writable_buffer* response_data) {
  struct edh_srv_message_1_error_context error_ctx = {0};

  if (prepare_message_1_error_context(edhoc_api_result, edhoc_context,
                                      generic_error_message, &error_ctx) !=
      EDH_SRV_PREP_M1_ERROR_CTX_OK) {
    return EDH_SRV_MSG1_ADD_ERROR_ERR_PREPARE_CTX;
  }
  if (edh_com_add_edhoc_error_to_response(edhoc_api_result, &error_ctx.info,
                                          response_data) !=
      EDH_COM_ADD_ERROR_OK) {
    return EDH_SRV_MSG1_ADD_ERROR_ERR_ADD_ERROR_TO_RESPONSE;
  }
  return EDH_SRV_MSG1_ADD_ERROR_OK;
}
