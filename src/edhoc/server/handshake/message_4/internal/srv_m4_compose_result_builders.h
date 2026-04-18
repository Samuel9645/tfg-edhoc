/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Declarations for the srv_m4_compose_result_builders module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_4_INTERNAL_SRV_M4_COMPOSE_RESULT_BUILDERS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_4_INTERNAL_SRV_M4_COMPOSE_RESULT_BUILDERS_H_

#include "edhoc/server/handshake/message_4/srv_m4_compose_result.h"

struct srv_edhoc_message_4_compose_result
srv_edhoc_message_4_compose_invalid_compose_buffer_failure(void);

struct srv_edhoc_message_4_compose_result srv_edhoc_message_4_compose_failure(
    enum srv_edhoc_message_4_compose_status status,
    struct com_readonly_buffer error);

struct srv_edhoc_message_4_compose_result srv_edhoc_message_4_compose_ok(
    struct com_readonly_buffer message_4);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_4_INTERNAL_SRV_M4_COMPOSE_RESULT_BUILDERS_H_

