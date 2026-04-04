/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Scenario table definition
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
#ifndef EDHOC_SERVER_HANDSHAKE_SCENARIOS_H_
#define EDHOC_SERVER_HANDSHAKE_SCENARIOS_H_

#include "edhoc/server/handshake/message_1/srv_m1_handler.h"

/**
 * @brief Represents a single test case for Message 1 handling, including input
 * request data and expected response buffer state.
 */
typedef struct tst_edh_srv_hnd_test_case_message_1_handler {
  /* Description of the test case for debugging purposes. */
  const char* description;
  /* Input request data for Message 1 handler. */
  const edh_srv_message_1_request_t* request;
  /* Response buffer to check for side effects after handler execution. */
  com_writable_buffer_t* response;
} tst_edh_srv_hnd_test_case_message_1_handler_t;

#endif  // EDHOC_SERVER_HANDSHAKE_SCENARIOS_H_