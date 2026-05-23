/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 23/05/2026
 * @brief Definition for the module to add cipher suite errors.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/common/add_error/com_edhoc_add_cipher_suite_mismatch_error.h"

#include "edhoc/common/add_error/com_edhoc_add_protocol_error.h"

enum { SRV_EDHOC_CIPHER_SUITES_ARRAY_SIZE = 8 };

struct com_readonly_buffer com_edhoc_add_cipher_suite_mismatch_error(
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