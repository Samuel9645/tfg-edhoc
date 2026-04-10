/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 05/04/2026
 * @brief Helper functions to build results of handling message 1 EDHOC requests
 */

#include "srv_m1_handler_result_builders.h"

#include <stddef.h>

struct srv_edhoc_message_1_handler_result srv_edhoc_message_1_handler_ok(
    struct edhoc_context* edhoc_ctx) {
  return (struct srv_edhoc_message_1_handler_result){
      .status = SRV_EDHOC_MSG1_HDL_OK, .edhoc_ctx = edhoc_ctx};
}

struct srv_edhoc_message_1_handler_result srv_edhoc_message_1_handler_failure(
    const enum srv_edhoc_message_1_handler_status status) {
  return (struct srv_edhoc_message_1_handler_result){.status = status,
                                                     .edhoc_ctx = NULL};
}