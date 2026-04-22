/**
 * @file
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

enum cli_edhoc_init_status cli_edhoc_init_handshake(
    struct cli_edhoc_handshake* state,
    const struct edhoc_credentials* credentials) {
  return cli_edhoc_init(state, credentials);
}

struct cli_edhoc_message_1_compose_result cli_edhoc_handshake_compose_message_1(
    struct cli_edhoc_handshake* state, struct com_writable_buffer* message_1) {
  return cli_edhoc_compose_message_1(state, message_1);
}

struct cli_edhoc_message_2_process_result cli_edhoc_handshake_process_message_2(
    struct cli_edhoc_handshake* state,
    const struct com_readonly_buffer message_2,
    struct com_writable_buffer* message_2_error) {
  return cli_edhoc_process_message_2(state, message_2_error);
}

struct cli_edhoc_message_3_compose_result cli_edhoc_handshake_compose_message_3(
    struct cli_edhoc_handshake* state, struct com_writable_buffer* message_3) {
  return cli_edhoc_compose_message_3(state, message_3);
}

struct cli_edhoc_message_4_process_result cli_edhoc_handshake_process_message_4(
    struct cli_edhoc_handshake* state,
    const struct com_readonly_buffer message_4,
    struct com_writable_buffer* message_4_error) {
  return cli_edhoc_process_message_4(state, message_4, message_4_error);
}
