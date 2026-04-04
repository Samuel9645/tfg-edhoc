/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Helper function implementations for EDHOC server handshake tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppDFAConstantParameter
#include <edhoc.h>
#include <string.h>
#include <unity.h>

#include "edhoc/server/handshake/common/tst_srv_m1_helpers.h"
#include "edhoc/server/handshake/message_1/handler/tst_m1_hdl_helpers.h"

static const struct edhoc_credentials DUMMY_TEST_CREDS = {0};
static const uint8_t CLEAN_MESSAGE_1_PAYLOAD[] = {0x01, 0x02};
static const size_t CLEAN_MESSAGE_1_PAYLOAD_SIZE =
    sizeof(CLEAN_MESSAGE_1_PAYLOAD);

void tst_edh_setup_message_1_handler_env(tst_message_1_handler_env_t* env) {
  TEST_ASSERT_NOT_NULL_MESSAGE(env,
                               "Test environment pointer must not be NULL");

  *env = (tst_message_1_handler_env_t){0};
  memcpy(env->request_payload, CLEAN_MESSAGE_1_PAYLOAD,
         CLEAN_MESSAGE_1_PAYLOAD_SIZE);
  env->request = (edh_srv_message_1_request_t){
      .payload =
          (com_readonly_buffer_t){.bytes = env->request_payload,
                                  .length = CLEAN_MESSAGE_1_PAYLOAD_SIZE},
      .credentials = &DUMMY_TEST_CREDS};
  env->response =
      (com_writable_buffer_t){.bytes = env->response_payload,
                              .capacity = sizeof(env->response_payload)};
  tst_edh_reset_message_1_response(&env->response);
}

void tst_edh_reset_message_1_response(com_writable_buffer_t* response) {
  TEST_ASSERT_NOT_NULL(response);
  TEST_ASSERT_NOT_NULL(response->bytes);

  response->length = TST_NONZERO_VALUE;
  memset(response->bytes, TST_CANARY_PATTERN, response->capacity);
}