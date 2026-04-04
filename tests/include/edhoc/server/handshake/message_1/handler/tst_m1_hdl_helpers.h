/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Helper functions for EDHOC server handshake unit tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#ifndef EDHOC_SERVER_HANDSHAKE_HELPERS_H_
#define EDHOC_SERVER_HANDSHAKE_HELPERS_H_

#include "edhoc/server/handshake/message_1/srv_m1_handler.h"

enum { TST_EDH_SRV_HND_BUF_LEN = 256 };

typedef struct tst_message_1_handler_env {
  uint8_t request_payload[TST_EDH_SRV_HND_BUF_LEN];
  uint8_t response_payload[TST_EDH_SRV_HND_BUF_LEN];
  size_t response_written_len;
  edh_srv_message_1_request_t request;
  com_writable_buffer_t response;
} tst_message_1_handler_env_t;

/**
 * @brief Overrides the request payload in the test environment with new data.
 * @param env Pointer to the handshake_test_env_t containing the request to
 * modify.
 * @param new_payload Pointer to the new payload data to set in the request.
 * @param new_len Length of the new payload data in bytes; must not exceed
 * EDH_SRV_HND_TST_BUF_LEN.
 */
void tst_edh_override_message_1_handler_request(
    tst_message_1_handler_env_t* env, const uint8_t* new_payload,
    size_t new_len);

/**
 * @brief Creates a testing environment for EDHOC server handshake tests.
 * @param[out] env Pointer to uninitialized handshake_test_env_t struct to
 * populate.
 */
void tst_edh_setup_message_1_handler_env(tst_message_1_handler_env_t* env);

/**
 * @brief Resets the response buffer in the test environment to a known state.
 * @param[in,out] response Pointer to the com_response_buffer_t to
 * reset.
 */
void tst_edh_reset_message_1_response(com_writable_buffer_t* response);

/**
 * @brief Asserts that the response buffer has not been modified (i.e., no
 * side effects) after a handler call.
 * @param[in] response Pointer to the com_response_buffer_t to check.
 */
void tst_edh_assert_message_1_handler_response_clean(
    const com_writable_buffer_t* response);

/**
 * @brief Sets the payload for a valid Message 1 in the test environment.
 * @param[out] buffer Pointer to the buffer where the payload will be stored.
 * @param[in] buffer_capacity The maximum capacity of the buffer.
 * @param[out] written_len Pointer to the variable that will store the length of
 * the written payload.
 */
void tst_edh_set_valid_message_1_request(uint8_t* buffer,
                                         size_t buffer_capacity,
                                         size_t* written_len);

#endif  // EDHOC_SERVER_HANDSHAKE_HELPERS_H_