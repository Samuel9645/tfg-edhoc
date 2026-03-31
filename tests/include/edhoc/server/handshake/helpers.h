/**
 * @file helpers.h
 *
 * @brief Helper functions for EDHOC server handshake unit tests.
 */

#ifndef EDHOC_SERVER_HANDSHAKE_HELPERS_H_
#define EDHOC_SERVER_HANDSHAKE_HELPERS_H_

#include "edhoc/server/handshake.h"

enum { TEDSH_MESSAGE_BUFFER_LENGTH = 256 };
typedef struct {
  int session_dummy;
  int pdu_dummy;
  uint8_t req_payload[TEDSH_MESSAGE_BUFFER_LENGTH];
  uint8_t res_payload[TEDSH_MESSAGE_BUFFER_LENGTH];
  size_t res_written_len;
  edhoc_server_common_request_data_t request;
  common_response_buffer_t response;
} handshake_test_env_t;

/**
 * @brief Creates a testing environment for EDHOC server handshake tests.
 * @param[out] env Pointer to uninitialized handshake_test_env_t struct to
 * populate.
 */
void setup_testing_environment(handshake_test_env_t* env);

/**
 * @brief Resets the response buffer in the test environment to a known state.
 * @param[in,out] response_data Pointer to the common_response_buffer_t to
 * reset.
 */
void reset_test_response(common_response_buffer_t* response_data);

/**
 * @brief Asserts that the response buffer has not been modified (i.e., no
 * side-effects) after a handler call.
 * @param[in] response Pointer to the common_response_buffer_t to check.
 */
void assert_response_untouched(const common_response_buffer_t* response);

/**
 * @brief Sets the payload for a valid Message 1 in the test environment.
 * @param[out] buffer Pointer to the buffer where the payload will be stored.
 * @param[in] buffer_capacity The maximum capacity of the buffer.
 * @param[out] written_len Pointer to the variable that will store the length of
 * the written payload.
 */
void set_valid_message_1_payload(uint8_t* buffer, size_t buffer_capacity,
                                 size_t* written_len);

#endif  // EDHOC_SERVER_HANDSHAKE_HELPERS_H_