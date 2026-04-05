/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of handling message 1 EDHOC requests
 */

#include "srv_m1_handler_result_builders.h"

#include <stddef.h>

struct ehd_srv_message_1_handler_result edh_srv_message_1_handler_ok(
    struct edhoc_context* edhoc_ctx) {
  return (struct ehd_srv_message_1_handler_result){
      .status = EDH_SRV_MSG1_HDL_OK, .edhoc_ctx = edhoc_ctx};
}

struct ehd_srv_message_1_handler_result edh_srv_message_1_handler_failure(
    const enum edh_srv_message_1_handler_status status) {
  return (struct ehd_srv_message_1_handler_result){.status = status,
                                                   .edhoc_ctx = NULL};
}