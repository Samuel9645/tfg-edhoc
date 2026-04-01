/**
 * @file handle_libedhoc_errors.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since Mon 30 Mar
 * @brief Functions to write the edhoc rfc codes from the library into the
 * response
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handle_libedhoc_errors.h"

#include <string.h>

enum { EDSH_CIPHER_SUITES_ARRAY_SIZE = 8 };

/**
 * @brief Prepares the edhoc_error_info struct with a generic error message.
 * @param[in] error_message error message to include in the error info
 * struct
 * @param[out] error_info error info struct to populate with the provided
 * error message
 */
static void prepare_generic_error_info(char* error_message,
                                       struct edhoc_error_info* error_info) {
  memset(error_info, 0, sizeof(struct edhoc_error_info));
  error_info->text_string = error_message;
  error_info->total_entries = strlen(error_message);
}

/**
 * @brief Prepares the edhoc_error_info struct based on the error code.
 * @param[in] context The current EDHOC context
 * @param[out] own_suites_buf Buffer provided by caller to hold supported
 * suites
 * @param[in] own_suites_capacity Maximum number of suites the buffer can hold
 * @param[out] error_info The struct to populate
 */
static void prepare_wrong_cipher_suite_error_info(
    const struct edhoc_context* context, int32_t* own_suites_buf,
    const size_t own_suites_capacity, struct edhoc_error_info* error_info) {
  memset(error_info, 0, sizeof(struct edhoc_error_info));
  // TODO: maybe log the peer suites as well for debugging
  int32_t peer_suites[EDSH_CIPHER_SUITES_ARRAY_SIZE] = {0};
  size_t peer_len = 0;
  size_t own_len = 0;
  edhoc_error_get_cipher_suites(context, own_suites_buf, own_suites_capacity,
                                &own_len, peer_suites, ARRAY_SIZE(peer_suites),
                                &peer_len);
  error_info->cipher_suites = own_suites_buf;
  error_info->total_entries = own_len;
}

void server_edhoc_add_edhoc_error_to_response(
    const int edhoc_api_result, const struct edhoc_context* edhoc_ctx,
    common_response_buffer_t* response_data) {
  size_t error_payload_len = 0;
  struct edhoc_error_info error_info = {0};
  if (edhoc_api_result == EDHOC_ERROR_CODE_WRONG_SELECTED_CIPHER_SUITE) {
    int32_t own_suites[EDSH_CIPHER_SUITES_ARRAY_SIZE] = {0};
    prepare_wrong_cipher_suite_error_info(
        edhoc_ctx, own_suites, EDSH_CIPHER_SUITES_ARRAY_SIZE, &error_info);
  } else {
    prepare_generic_error_info("Message 1 processing failed", &error_info);
  }
  edhoc_message_error_compose(
      response_data->payload, response_data->payload_capacity,
      &error_payload_len, EDHOC_ERROR_CODE_UNSPECIFIED_ERROR, &error_info);
  response_data->payload_length = error_payload_len;
}
