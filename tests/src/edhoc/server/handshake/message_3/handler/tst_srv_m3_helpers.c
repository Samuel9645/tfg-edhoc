/**
 * @file helpers.c
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

void tst_edh_srv_message_3_setup_env(tst_edh_srv_message_3_env_t* env) {
  TEST_ASSERT_NOT_NULL_MESSAGE(env,
                               "Test environment pointer must not be NULL");

  *env = (tst_edh_srv_message_3_env_t){0};
  memcpy(env->request_payload, CLEAN_MESSAGE_3_PAYLOAD,
         CLEAN_MESSAGE_3_PAYLOAD_SIZE);
  env->extracted_fields = (struct edhoc_extracted_fields){
      .buffer = env->request_payload,
      .buffer_size = CLEAN_MESSAGE_3_PAYLOAD_SIZE,
      .edhoc_message_ptr = env->request_payload,
      .edhoc_message_size = CLEAN_MESSAGE_3_PAYLOAD_SIZE,
  };
  env->request = (edh_srv_message_3_request_t){
      .edhoc_ctx = (struct edhoc_context*)&env->context_dummy,
      .message_3_extracted_fields = &env->extracted_fields,
  };
  env->response.bytes = env->response_payload;
  env->response.capacity = sizeof(env->response_payload);
  tst_edh_reset_response(&env->response);
}

edh_srv_message_3_request_t tst_message_3_request_without_extracted_fields(
    const tst_edh_srv_message_3_env_t* env) {
  edh_srv_message_3_request_t request = env->request;
  request.message_3_extracted_fields = NULL;
  return request;
}

edh_srv_message_3_request_t tst_message_3_request_without_context(

    const tst_edh_srv_message_3_env_t* env) {
  edh_srv_message_3_request_t request = env->request;
  request.edhoc_ctx = NULL;
  return request;
}

edh_srv_message_3_request_t tst_empty_message_3_request(void) {
  return (edh_srv_message_3_request_t){0};
}

