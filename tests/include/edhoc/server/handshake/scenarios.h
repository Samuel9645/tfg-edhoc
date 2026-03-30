/**
 * @file scenarios.h
 *
 * @brief Scenario table definition
 */
#ifndef EDHOC_SERVER_HANDSHAKE_SCENARIOS_H_
#define EDHOC_SERVER_HANDSHAKE_SCENARIOS_H_

#include "edhoc/server/handshake.h"

/**
 * @brief Represents a single test case for Message 1 handling, including input
 * request data and expected response buffer state.
 */
typedef struct handshake_test_case {
  /* Description of the test case for debugging purposes. */
  const char* description;
  /* Input request data for Message 1 handler. */
  const edhoc_server_common_request_data_t* request;
  /* Response buffer to check for side-effects after handler execution. */
  common_response_buffer_t* response;
} handshake_test_case_t;

#endif  // EDHOC_SERVER_HANDSHAKE_SCENARIOS_H_