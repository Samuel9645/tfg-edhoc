/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 16/04/2026
 * @brief Declarations for the srv_m2_compose_result_builders module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_2_INTERNAL_SRV_M2_COMPOSE_RESULT_BUILDERS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_2_INTERNAL_SRV_M2_COMPOSE_RESULT_BUILDERS_H_

#include "edhoc/server/handshake/message_2/srv_m2_compose_result.h"

struct srv_edhoc_message_2_compose_result
srv_edhoc_message_2_compose_invalid_compose_buffer_failure(void);

struct srv_edhoc_message_2_compose_result srv_edhoc_message_2_compose_failure(
    enum srv_edhoc_message_2_compose_status status,
    struct com_readonly_buffer error);

struct srv_edhoc_message_2_compose_result srv_edhoc_message_2_compose_ok(
    struct com_readonly_buffer message_2);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_2_INTERNAL_SRV_M2_COMPOSE_RESULT_BUILDERS_H_
