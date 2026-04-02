/**
 * @file result.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 01/04/2026
 * @brief EDHOC Message 1 result constructors.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#include "edhoc/server/handshake/message_1/result.h"

#include <stddef.h>

ehd_message_1_handler_result_t edh_message_1_handler_ok(
    struct edhoc_context* edhoc_ctx) {
  return (ehd_message_1_handler_result_t){.status = EDH_MSG1_HDL_OK,
                                          .edhoc_ctx = edhoc_ctx};
}

ehd_message_1_handler_result_t edh_message_1_handler_failure(
    const edh_message_1_handler_status_t status) {
  return (ehd_message_1_handler_result_t){.status = status, .edhoc_ctx = NULL};
}