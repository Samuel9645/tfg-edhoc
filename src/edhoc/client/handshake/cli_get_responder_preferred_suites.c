/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/05/2026
 * @brief Module to get the responder's preferred cipher suites from an error
 * buffer
 * @see [RFC
 * 9528 6.3.1](https://datatracker.ietf.org/doc/html/rfc9528/#name-cipher-suite-negotiation)
 */
#include "edhoc/client/handshake/cli_get_responder_preferred_suites.h"

#include <edhoc.h>

static struct cli_edhoc_responder_preferred_suites_result ok(
    const struct com_edhoc_cipher_suite_details* preferred_suite) {
  return (struct cli_edhoc_responder_preferred_suites_result){
      .status = CLI_EDHOC_RESP_PREFERRED_SUITES_OK,
      .preferred_suite = preferred_suite,
  };
}

static struct cli_edhoc_responder_preferred_suites_result no_common_suites(
    void) {
  return (struct cli_edhoc_responder_preferred_suites_result){
      .status = CLI_EDHOC_RESP_PREFERRED_SUITES_NO_COMMON_SUITES};
}

static struct cli_edhoc_responder_preferred_suites_result failure(
    const enum cli_edhoc_responder_preferred_suites_status status) {
  return (struct cli_edhoc_responder_preferred_suites_result){.status = status};
}

enum { CLI_GET_RESPONDER_SUITES_ERROR_SIZE = 100 };

struct cli_edhoc_responder_preferred_suites_result
cli_edhoc_get_responder_preferred_suites(
    const struct com_edhoc_cipher_suite_list* own_supported_suites,
    const struct com_readonly_buffer encoded_error_buffer) {
  if (!com_readonly_buffer_has_content(encoded_error_buffer)) {
    return failure(CLI_EDHOC_RESP_PREFERRED_SUITES_ERR_EMPTY_ERROR_BUFFER);
  }
  if (!com_edhoc_cipher_suites_are_valid(own_supported_suites)) {
    return failure(
        CLI_EDHOC_RESP_PREFERRED_SUITES_ERR_INVALID_SUPPORTED_SUITES);
  }

  enum edhoc_error_code received_code = -1;
  char decoded_error[CLI_GET_RESPONDER_SUITES_ERROR_SIZE] = {0};
  struct edhoc_error_info received_info = {
      .text_string = decoded_error, .total_entries = sizeof(decoded_error)};
  edhoc_message_error_process(encoded_error_buffer.bytes,
                              encoded_error_buffer.length, &received_code,
                              &received_info);
  for (size_t i = 0; i < received_info.written_entries; i++) {
    const struct com_edhoc_cipher_suite_details* details =
        com_edhoc_get_cipher_suite_from_value(received_info.cipher_suites[i]);
    if (details == NULL) {
      continue;
    }
    for (size_t j = 0; j < own_supported_suites->number_of_suites; j++) {
      if (own_supported_suites->suites[j].metadata->value ==
          details->metadata->value) {
        return ok(details);
      }
    }
  }
  return no_common_suites();
}