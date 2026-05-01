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

struct cli_edhoc_responder_preferred_suites_result
cli_edhoc_get_responder_preferred_suites(
    const struct com_edhoc_cipher_suite_list* own_supported_suites,
    const struct com_readonly_buffer encoded_error_buffer) {
  (void)own_supported_suites;
  (void)encoded_error_buffer;
  return ok(NULL);
}