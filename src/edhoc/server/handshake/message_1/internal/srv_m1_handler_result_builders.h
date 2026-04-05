/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of parsing message 1 EDHOC requests
 */

#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_1_INTERNAL_SRV_M1_HANDLER_RESULT_BUILDERS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_1_INTERNAL_SRV_M1_HANDLER_RESULT_BUILDERS_H_
#include "edhoc/server/handshake/message_1/srv_m1_result.h"

ehd_srv_message_1_handler_result_t edh_srv_message_1_handler_ok(
    struct edhoc_context* edhoc_ctx);

ehd_srv_message_1_handler_result_t edh_srv_message_1_handler_failure(
    edh_srv_message_1_handler_status_t status);

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_1_INTERNAL_SRV_M1_HANDLER_RESULT_BUILDERS_H_