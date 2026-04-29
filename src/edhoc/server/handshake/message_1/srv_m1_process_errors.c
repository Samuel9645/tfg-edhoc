/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief Message 1 specific EDHOC error-response composition.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_1/srv_m1_process_errors.h"

#include <edhoc_helpers.h>

#include "edhoc/common/add_error/com_edhoc_add_internal_error.h"
#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

struct com_readonly_buffer srv_edhoc_message_1_process_add_protocol_error(
    const struct edhoc_context* context, const char* error_description,
    const struct com_writable_buffer response_data) {
  enum edhoc_error_code error;
  if (edhoc_error_get_code(context, &error) != EDHOC_SUCCESS) {
    error = EDHOC_ERROR_CODE_UNSPECIFIED_ERROR;
  }
  if (error == EDHOC_ERROR_CODE_WRONG_SELECTED_CIPHER_SUITE) {
    enum { SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE = 8 };

    int32_t peer_suites[SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE] = {0};
    size_t peer_length = 0;
    int32_t own_suites[SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE] = {0};
    size_t own_length = 0;
    if (edhoc_error_get_cipher_suites(
            context, own_suites, SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE,
            &own_length, peer_suites, SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE,
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
    return com_edhoc_add_protocol_error_view(context, &error_info,
                                             response_data);
  }
  return com_edhoc_add_protocol_error_with_description_view(
      context, error_description, response_data);
}
