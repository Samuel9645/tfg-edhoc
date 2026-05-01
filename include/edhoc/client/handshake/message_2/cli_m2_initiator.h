/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 22/04/2026
 * @brief Logic for responding to Message 2 (process Message 2 and compose
 * Message 3)
 * @see [RFC 9528
 * A.2.1](https://datatracker.ietf.org/doc/html/rfc9528/#name-the-forward-message-flow)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_INITIATOR_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_INITIATOR_H_

#include <edhoc.h>

#include "common/com_data_models.h"
#include "edhoc/client/handshake/message_2/cli_m2_initiator_result.h"

struct cli_edhoc_message_2_initiator_request {
  struct com_readonly_buffer raw_payload;
  struct edhoc_context* edhoc_context;
};

/**
 * @brief Process Message 2 and compose Message 3 into a writable buffer view.
 * @param[in] request Message 2 request metadata and EDHOC context.
 * @param[in] response_buffer Metadata of the buffer (pointer and capacity) to
 * write into
 */
struct cli_edhoc_message_2_initiator_result cli_edhoc_respond_to_message_2(
    struct cli_edhoc_message_2_initiator_request request,
    struct com_writable_buffer response_buffer);

const char* cli_edhoc_respond_to_message_2_status_code_to_string(
    enum cli_edhoc_message_2_initiator_status status);

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_INITIATOR_H_
