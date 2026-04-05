/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of handling message 1 EDHOC requests
 */

#include "srv_m1_handler_result_builders.h"

#include <stddef.h>

ehd_srv_message_1_handler_result_t edh_srv_message_1_handler_ok(
    struct edhoc_context* edhoc_ctx) {
  return (ehd_srv_message_1_handler_result_t){.status = EDH_SRV_MSG1_HDL_OK,
                                              .edhoc_ctx = edhoc_ctx};
}

ehd_srv_message_1_handler_result_t edh_srv_message_1_handler_failure(
    const edh_srv_message_1_handler_status_t status) {
  return (ehd_srv_message_1_handler_result_t){.status = status,
                                              .edhoc_ctx = NULL};
}