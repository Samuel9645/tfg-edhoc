/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/05/2026
 * @brief Module to negotiate the new suites to include in the Message 1
 * @see [RFC
 * 9528 6.3.1](https://datatracker.ietf.org/doc/html/rfc9528/#name-cipher-suite-negotiation)
 */
#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_GET_RESPONDER_PREFERRED_SUITES_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_GET_RESPONDER_PREFERRED_SUITES_H_
#include "common/com_data_models.h"
#include "edhoc/common/com_edhoc_cipher_suites.h"

enum cli_edhoc_suites_negotiation_status {
  CLI_EDHOC_NEGOTIATE_SUITES_OK = 0,
  CLI_EDHOC_NEGOTIATE_SUITES_NO_COMMON_SUITES,
  CLI_EDHOC_NEGOTIATE_SUITES_ERR_EMPTY_ERROR_BUFFER,
  CLI_EDHOC_NEGOTIATE_SUITES_ERR_INVALID_SUPPORTED_SUITES,
  CLI_EDHOC_NEGOTIATE_SUITES_ERR_INTERNAL_BUFFER_TOO_SMALL,
  CLI_EDHOC_NEGOTIATE_SUITES_ERR_PROCESSING_ERROR,
};

enum { MAX_RENEGOTIATION_SIZE = 10 };

struct cli_edhoc_renegotiation_list {
  const struct com_edhoc_cipher_suite_details* suites[MAX_RENEGOTIATION_SIZE];
  size_t number_of_suites;
};

struct cli_edhoc_suites_negotiation_result {
  const enum cli_edhoc_suites_negotiation_status status;
  struct cli_edhoc_renegotiation_list renegotiation_suites;
};

/**
 * @brief Checks if the Message 1 response error suggests renegotiation of
 * cipher suites
 * @param[in] encoded_error_buffer View of the error response to Message 1
 * @return true if error buffer contains cipher suite mismatch error, false
 * otherwise
 */
bool cli_edhoc_error_suggests_renegotiation(
    struct com_readonly_buffer encoded_error_buffer);

/**
 * @brief Negotiates suites based on the initiator supported suites and the
 * given responder suites inside the encoded error buffer.
 * @param[in] own_supported_suites List of cipher suites supported by the
 * initiator. They must be like in other cases in order of preference.
 * @param[in] encoded_error_buffer View of the error response to Message 1
 * @return Struct containing a status and the new list of suites, in order of
 * preference, with the last one being the selected. On failure the
 * list will be empty, and the status will indicate the error.
 */
struct cli_edhoc_suites_negotiation_result cli_edhoc_negotiate_suites(
    struct com_edhoc_cipher_suite_list own_supported_suites,
    struct com_readonly_buffer encoded_error_buffer);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_GET_RESPONDER_PREFERRED_SUITES_H_