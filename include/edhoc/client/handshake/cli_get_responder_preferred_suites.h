/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/05/2026
 * @brief Module to get the responder's preferred cipher suites from an error
 * buffer
 * @see [RFC
 * 9528 6.3.1](https://datatracker.ietf.org/doc/html/rfc9528/#name-cipher-suite-negotiation)
 */
#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_GET_RESPONDER_PREFERRED_SUITES_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_GET_RESPONDER_PREFERRED_SUITES_H_
#include "common/com_data_models.h"
#include "edhoc/common/com_edhoc_cipher_suites.h"

enum cli_edhoc_responder_preferred_suites_status {
  CLI_EDHOC_RESP_PREFERRED_SUITES_OK,
  CLI_EDHOC_RESP_PREFERRED_SUITES_NO_COMMON_SUITES,
  CLI_EDHOC_RESP_PREFERRED_SUITES_ERR_EMPTY_ERROR_BUFFER,
  CLI_EDHOC_RESP_PREFERRED_SUITES_ERR_INVALID_SUPPORTED_SUITES,
};

struct cli_edhoc_responder_preferred_suites_result {
  const enum cli_edhoc_responder_preferred_suites_status status;
  const struct com_edhoc_cipher_suite_details* preferred_suite;
};

struct cli_edhoc_responder_preferred_suites_result
cli_edhoc_get_responder_preferred_suites(
    const struct com_edhoc_cipher_suite_list* own_supported_suites,
    struct com_readonly_buffer encoded_error_buffer);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_GET_RESPONDER_PREFERRED_SUITES_H_