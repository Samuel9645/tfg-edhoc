/**
 * @file handshake.c
 * @author Samuel Rodriguez <alu0101545714@ull.edu.es>
 * @since 04/04/2026
 * @brief Public facade wrappers for modular client handshake
 * init/compose/process APIs.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/client/cli_handshake.h"
#include "edhoc/client/handshake/common/cli_state.h"
#include "edhoc/client/handshake/message_1/cli_m1_compose.h"
#include "edhoc/client/handshake/message_2/cli_m2_process.h"
#include "edhoc/client/handshake/message_3/cli_m3_compose.h"
#include "edhoc/client/handshake/message_4/cli_m4_process.h"

edh_cli_init_status_t edh_cli_init_handshake(
    edh_cli_handshake_t* state, const struct edhoc_credentials* credentials) {
  return edh_cli_init(state, credentials);
}

edh_cli_message_1_result_t edh_cli_handshake_compose_message_1(
    edh_cli_handshake_t* state, com_writable_buffer_t* message_1) {
  return edh_cli_compose_message_1(state, message_1);
}

edh_cli_message_2_result_t edh_cli_handshake_process_message_2(
    edh_cli_handshake_t* state, const com_readonly_buffer_t* message_2,
    com_writable_buffer_t* message_2_error) {
  return edh_cli_process_message_2(state, message_2, message_2_error);
}

edh_cli_message_3_result_t edh_cli_handshake_compose_message_3(
    edh_cli_handshake_t* state, com_writable_buffer_t* message_3) {
  return edh_cli_compose_message_3(state, message_3);
}

edh_cli_message_4_result_t edh_cli_handshake_process_message_4(
    edh_cli_handshake_t* state, const com_readonly_buffer_t* message_4,
    com_writable_buffer_t* message_4_error) {
  return edh_cli_process_message_4(state, message_4, message_4_error);
}
