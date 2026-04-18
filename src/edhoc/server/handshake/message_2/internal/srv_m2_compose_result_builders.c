/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 16/04/2026
 * @brief Definitions for the srv_m2_compose_result_builders module.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/message_2/internal/srv_m2_compose_result_builders.h"

struct srv_edhoc_message_2_compose_result
srv_edhoc_message_2_compose_invalid_compose_buffer_failure(void) {
  return (struct srv_edhoc_message_2_compose_result){
      .status = SRV_EDHOC_MSG2_COMPOSE_ERR_INVALID_COMPOSE_BUFFER};
}

struct srv_edhoc_message_2_compose_result srv_edhoc_message_2_compose_failure(
    const enum srv_edhoc_message_2_compose_status status,
    const struct com_readonly_buffer error) {
  return (struct srv_edhoc_message_2_compose_result){.status = status,
                                                     .buffer = error};
}

struct srv_edhoc_message_2_compose_result srv_edhoc_message_2_compose_ok(
    const struct com_readonly_buffer message_2) {
  return (struct srv_edhoc_message_2_compose_result){
      .status = SRV_EDHOC_MSG2_COMPOSE_OK, .buffer = message_2};
}