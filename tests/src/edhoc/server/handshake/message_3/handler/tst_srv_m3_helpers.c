/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Helper functions for EDHOC server message 3 handler tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include <string.h>
#include <unity.h>

#include "edhoc/server/handshake/common/tst_srv_m1_helpers.h"
#include "edhoc/server/handshake/message_3/handler/tst_m3_hdl_helpers.h"

static const uint8_t CLEAN_MESSAGE_3_PAYLOAD[] = {0x21, 0x22, 0x23};
static const size_t CLEAN_MESSAGE_3_PAYLOAD_SIZE =
    sizeof(CLEAN_MESSAGE_3_PAYLOAD);

void tst_edh_srv_message_3_setup_env(struct tst_edh_srv_message_3_env* env) {
  TEST_ASSERT_NOT_NULL_MESSAGE(env,
                               "Test environment pointer must not be NULL");
  *env = (struct tst_edh_srv_message_3_env){0};
  memcpy(env->request_payload, CLEAN_MESSAGE_3_PAYLOAD,
         CLEAN_MESSAGE_3_PAYLOAD_SIZE);
  env->request = (struct edh_srv_message_3_request){
      .edhoc_ctx = &env->context,  // Points to the actual context in the env
      .parsed_message_3 = {.bytes = env->request_payload,
                           .length = CLEAN_MESSAGE_3_PAYLOAD_SIZE}};
  env->response =
      (struct com_writable_buffer){.bytes = env->response_payload,
                                   .capacity = sizeof(env->response_payload),
                                   .length = 0};
}

struct edh_srv_message_3_request tst_message_3_request_without_buffer(
    const struct tst_edh_srv_message_3_env* env) {
  struct edh_srv_message_3_request request = env->request;
  request.parsed_message_3.bytes = NULL;
  request.parsed_message_3.length = 0;
  return request;
}

struct edh_srv_message_3_request tst_message_3_request_without_context(
    const struct tst_edh_srv_message_3_env* env) {
  struct edh_srv_message_3_request request = env->request;
  request.edhoc_ctx = NULL;
  return request;
}

struct edh_srv_message_3_request tst_empty_message_3_request(void) {
  return (struct edh_srv_message_3_request){0};
}