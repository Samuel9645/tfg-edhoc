/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 22/04/2026
 * @brief Results of Message 2 processing on client side
 * @see [RFC
 * 9528 5.2.2](https://datatracker.ietf.org/doc/html/rfc9528/#name-initiator-composition-of-me)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_PROCESS_RESULT_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_PROCESS_RESULT_H_

enum cli_edhoc_message_2_process_status {
  CLI_EDHOC_MSG2_PROCESS_OK = 0,
  CLI_EDHOC_MSG2_PROCESS_ERR_NULL_CONTEXT,
  CLI_EDHOC_MSG2_PROCESS_ERR_EMPTY_MESSAGE_2,
  CLI_EDHOC_MSG2_PROCESS_ERR_EDHOC_MESSAGE_2_PROCESS_FAILED,
};

struct cli_edhoc_message_2_process_result {
  const enum cli_edhoc_message_2_process_status status;
};

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_2_CLI_M2_PROCESS_RESULT_H_
