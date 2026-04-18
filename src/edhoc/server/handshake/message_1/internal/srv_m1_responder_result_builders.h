/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of parsing message 1 EDHOC requests
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_INTERNAL_SRV_M1_HANDLER_RESULT_BUILDERS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_INTERNAL_SRV_M1_HANDLER_RESULT_BUILDERS_H_
#include "edhoc/server/handshake/message_1/srv_m1_responder_result.h"

struct srv_edhoc_message_1_responder_result srv_edhoc_message_1_responder_ok(
    struct edhoc_context* context, struct com_readonly_buffer message_2);

struct srv_edhoc_message_1_responder_result
srv_edhoc_message_1_responder_invalid_response_buffer_failure(void);

struct srv_edhoc_message_1_responder_result
srv_edhoc_message_1_responder_failure(
    enum srv_edhoc_message_1_responder_status status,
    struct com_readonly_buffer error_message);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_INTERNAL_SRV_M1_HANDLER_RESULT_BUILDERS_H_