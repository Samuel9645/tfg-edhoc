/**
 * @file scenarios.h
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Scenario table definition for EDHOC server message 3 handler tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_SCENARIOS_H_
#define EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_SCENARIOS_H_

#include "edhoc/server/handshake/message_3/handler.h"

typedef struct tst_edh_srv_message_3_test_case {
  const char* description;
  const edh_srv_message_3_request_t* request;
  com_writable_buffer_t* response;
} message_3_handler_test_case_t;

#endif  // EDHOC_SERVER_HANDSHAKE_MESSAGE_3_HANDLER_SCENARIOS_H_
