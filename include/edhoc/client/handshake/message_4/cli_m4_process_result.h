/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 22/04/2026
 * @brief Results of Message 4 processing on client side
 * @see [RFC
 * 9528 5.5.3](https://datatracker.ietf.org/doc/html/rfc9528/#name-initiator-processing-of-mess)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_CLI_M4_PROCESS_RESULT_H_
#define EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_CLI_M4_PROCESS_RESULT_H_

enum cli_edhoc_message_4_process_status {
  CLI_EDHOC_MSG4_PROCESS_OK = 0,
  CLI_EDHOC_MSG4_PROCESS_ERR_NULL_CONTEXT,
  CLI_EDHOC_MSG4_PROCESS_ERR_EMPTY_MESSAGE_4,
  CLI_EDHOC_MSG4_PROCESS_ERR_EDHOC_MESSAGE_4_PROCESS_FAILED,
  CLI_EDHOC_MSG4_PROCESS_ERR_BUFFER_CONVERSION,
};

struct cli_edhoc_message_4_process_result {
  const enum cli_edhoc_message_4_process_status status;
};

#endif  // EDHOC_CLIENT_HANDSHAKE_MESSAGE_4_CLI_M4_PROCESS_RESULT_H_
