/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/05/2026
 * @brief Module to get the responder's preferred cipher suites from an error
 * buffer
 * @see [RFC
 * 9528 6.3.1](https://datatracker.ietf.org/doc/html/rfc9528/#name-cipher-suite-negotiation)
 */
#include "edhoc/client/handshake/cli_negotiate_cipher_suites.h"

#include <edhoc.h>

static struct cli_edhoc_suites_negotiation_result ok(
    const struct cli_edhoc_renegotiation_list renegotiation_suites) {
  return (struct cli_edhoc_suites_negotiation_result){
      .status = CLI_EDHOC_NEGOTIATE_SUITES_OK,
      .renegotiation_suites = renegotiation_suites,
  };
}

static struct cli_edhoc_suites_negotiation_result no_common_suites(void) {
  return (struct cli_edhoc_suites_negotiation_result){
      .status = CLI_EDHOC_NEGOTIATE_SUITES_NO_COMMON_SUITES};
}

static struct cli_edhoc_suites_negotiation_result failure(
    const enum cli_edhoc_suites_negotiation_status status) {
  return (struct cli_edhoc_suites_negotiation_result){.status = status};
}

enum { CLI_GET_RESPONDER_SUITES_ERROR_SIZE = 100 };

static struct cli_edhoc_renegotiation_list merge_suite_list(
    const struct com_edhoc_cipher_suite_list own_preferred_suites,
    const struct com_edhoc_cipher_suite_details* new_selected_suite) {
  struct cli_edhoc_renegotiation_list result = {0};
  size_t i = 0;
  for (; i < own_preferred_suites.number_of_suites; i++) {
    result.suites[i] = own_preferred_suites.suites[i];
  }
  result.suites[i] = new_selected_suite;
  result.number_of_suites = i + 1;
  return result;
}

static bool suite_identifier_in_error_info(
    const struct edhoc_error_info error_info, const int32_t suite_identifier) {
  for (size_t i = 0; i < error_info.written_entries; i++) {
    if (error_info.cipher_suites[i] == suite_identifier) {
      return true;
    }
  }
  return false;
}

struct cli_edhoc_suites_negotiation_result cli_edhoc_negotiate_suites(
    const struct com_edhoc_cipher_suite_list own_supported_suites,
    const struct com_readonly_buffer encoded_error_buffer) {
  if (!com_readonly_buffer_has_content(encoded_error_buffer)) {
    return failure(CLI_EDHOC_NEGOTIATE_SUITES_ERR_EMPTY_ERROR_BUFFER);
  }
  if (!com_edhoc_cipher_suites_are_valid(own_supported_suites)) {
    return failure(CLI_EDHOC_NEGOTIATE_SUITES_ERR_INVALID_SUPPORTED_SUITES);
  }
  if (own_supported_suites.number_of_suites > MAX_RENEGOTIATION_SIZE) {
    return failure(CLI_EDHOC_NEGOTIATE_SUITES_ERR_INTERNAL_BUFFER_TOO_SMALL);
  }

  enum edhoc_error_code received_code = -1;
  char decoded_error[CLI_GET_RESPONDER_SUITES_ERROR_SIZE] = {0};
  struct edhoc_error_info received_info = {
      .text_string = decoded_error, .total_entries = sizeof(decoded_error)};
  if (edhoc_message_error_process(encoded_error_buffer.bytes,
                                  encoded_error_buffer.length, &received_code,
                                  &received_info) != EDHOC_SUCCESS) {
    return failure(CLI_EDHOC_NEGOTIATE_SUITES_ERR_PROCESSING_ERROR);
  }
  for (size_t i = 0; i < own_supported_suites.number_of_suites; i++) {
    const struct com_edhoc_cipher_suite_details* current_suite =
        own_supported_suites.suites[i];
    if (suite_identifier_in_error_info(received_info,
                                       current_suite->metadata->value)) {
      const struct com_edhoc_cipher_suite_list preferred_suites = {
          .suites = &own_supported_suites.suites[0], .number_of_suites = i};
      return ok(merge_suite_list(preferred_suites, current_suite));
    }
  }
  return no_common_suites();
}

bool cli_edhoc_error_suggests_renegotiation(
    const struct com_readonly_buffer encoded_error_buffer) {
  enum edhoc_error_code received_code = -1;
  char decoded_error[CLI_GET_RESPONDER_SUITES_ERROR_SIZE] = {0};
  struct edhoc_error_info received_info = {
      .text_string = decoded_error, .total_entries = sizeof(decoded_error)};
  if (edhoc_message_error_process(encoded_error_buffer.bytes,
                                  encoded_error_buffer.length, &received_code,
                                  &received_info) != EDHOC_SUCCESS) {
    return false;
  }
  return received_code == EDHOC_ERROR_CODE_WRONG_SELECTED_CIPHER_SUITE;
}