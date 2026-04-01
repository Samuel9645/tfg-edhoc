/**
 * @file message_1_result.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief EDHOC Message 1 result constructors.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "edhoc/server/handshake/message_1_result.h"

#include <stddef.h>

edhoc_server_message_1_result_t edhoc_server_message_1_ok(
    struct edhoc_context* edhoc_ctx) {
  return (edhoc_server_message_1_result_t){.status = ESHM1_OK,
                                           .edhoc_ctx = edhoc_ctx};
}

edhoc_server_message_1_result_t edhoc_server_message_1_failure(
    const edhoc_server_message_1_status_t status) {
  return (edhoc_server_message_1_result_t){.status = status, .edhoc_ctx = NULL};
}