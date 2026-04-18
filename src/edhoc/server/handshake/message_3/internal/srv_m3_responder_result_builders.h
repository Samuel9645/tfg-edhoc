/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Declarations for the srv_m3_responder_result_builders module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_INTERNAL_SRV_M3_RESPONDER_RESULT_BUILDERS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_INTERNAL_SRV_M3_RESPONDER_RESULT_BUILDERS_H_

#include "edhoc/server/handshake/message_3/srv_m3_responder_result.h"

struct srv_edhoc_message_3_responder_result
srv_edhoc_message_3_responder_invalid_response_buffer_failure(void);

struct srv_edhoc_message_3_responder_result
srv_edhoc_message_3_responder_failure(
    enum srv_edhoc_message_3_responder_status status,
    struct com_readonly_buffer error_buffer);

struct srv_edhoc_message_3_responder_result srv_edhoc_message_3_responder_ok(
    struct com_readonly_buffer message_4);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_INTERNAL_SRV_M3_RESPONDER_RESULT_BUILDERS_H_
