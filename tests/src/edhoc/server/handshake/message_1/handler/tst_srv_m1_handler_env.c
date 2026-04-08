/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Helper function implementations for EDHOC server handshake tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppDFAConstantParameter

#include "edhoc/server/handshake/message_1/handler/tst_srv_m1_handler_env.h"

#include <edhoc.h>
#include <string.h>
#include <unity.h>

static const struct edhoc_credentials DUMMY_TEST_CREDS = {0};
static const uint8_t DUMMY_MESSAGE_1_PAYLOAD[] = {0x01, 0x02};
static const size_t DUMMY_MESSAGE_1_PAYLOAD_SIZE =
    sizeof(DUMMY_MESSAGE_1_PAYLOAD);

void tst_edh_clear_message_1_setup_env(struct tst_message_1_handler_env* env) {
  TEST_ASSERT_NOT_NULL_MESSAGE(env,
                               "Test environment pointer must not be NULL");

  memset(env, 0, sizeof(*env));
  memcpy(env->request_payload, DUMMY_MESSAGE_1_PAYLOAD,
         DUMMY_MESSAGE_1_PAYLOAD_SIZE);
  env->valid_request = (struct edh_srv_message_1_request){
      .credentials = &DUMMY_TEST_CREDS,
      .payload = {.bytes = env->request_payload,
                  .length = DUMMY_MESSAGE_1_PAYLOAD_SIZE}};
  env->response =
      (struct com_writable_buffer){.bytes = env->response_payload,
                                   .capacity = sizeof(env->response_payload)};
}
