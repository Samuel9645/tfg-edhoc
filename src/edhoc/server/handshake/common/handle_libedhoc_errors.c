/**
 * @file handle_libedhoc_errors.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Shared functions to compose EDHOC error payloads into response
 * buffers.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/common/handle_libedhoc_errors.h"

#include <string.h>

// TODO: do not simply return, add error code
void edh_srv_set_error_info(const char* error_message,
                            struct edhoc_error_info* error_info) {
  if (!error_info || !error_message) {
    return;
  }
  memset(error_info, 0, sizeof(struct edhoc_error_info));
  const size_t len = strlen(error_message);
  if (len == 0) {
    return;
  }

  /* WARNING: As of libedhoc v1.0 (2025-04-14), text_string is treated as
   * read-only. The (char*) cast is required by the struct definition but
   * is safe for now. If libedhoc is updated, consider strdup() or stack
   * buffering. */
  error_info->text_string = (char*)error_message;
  error_info->total_entries = len;
  error_info->written_entries = len;
}

void edh_srv_add_edhoc_error_to_response(
    const int edhoc_api_result, const struct edhoc_error_info* error_info,
    com_response_buffer_t* response_data) {
  if (!response_data || !response_data->payload) {
    return;
  }

  size_t error_payload_len = 0;
  edhoc_message_error_compose(response_data->payload,
                              response_data->payload_capacity,
                              &error_payload_len, edhoc_api_result, error_info);
  response_data->payload_length = error_payload_len;
}
