/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 1 specific EDHOC error-response composition.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/srv_m1_errors.h"

#include <edhoc_helpers.h>

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

void srv_edhoc_message_1_handler_add_protocol_error(
    const struct edhoc_context* context, const char* generic_error_message,
    struct com_writable_buffer* response_data) {
  enum edhoc_error_code error;
  if (edhoc_error_get_code(context, &error) != EDHOC_SUCCESS) {
    error = EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;
  }
  if (error == EDHOC_ERROR_CODE_WRONG_SELECTED_CIPHER_SUITE) {
    enum { SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE = 8 };

    int32_t peer_suites[SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE] = {0};
    size_t peer_len = 0;
    int32_t own_suites[SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE] = {0};
    size_t own_len = 0;
    if (edhoc_error_get_cipher_suites(
            context, own_suites, SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE, &own_len,
            peer_suites, SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE,
            &peer_len) != EDHOC_SUCCESS) {
      return;
    }
    /*
     * Point where responder can compare his and peer cipher suites.
     * After comparison responder is able to send error message with his
     * preferences.
     */
    return;
  }
  (void)com_edhoc_add_protocol_error_with_description(
      context, generic_error_message, response_data);
}
