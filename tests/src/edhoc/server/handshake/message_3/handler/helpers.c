/**
 * @file helpers.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 02/04/2026
 * @brief Helper functions for EDHOC server message 3 handler tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "../../../../../../include/edhoc/server/handshake/message_3/handler/helpers.h"

#include <string.h>
#include <unity.h>

static const uint8_t CLEAN_MESSAGE_3_PAYLOAD[] = {0x21, 0x22, 0x23};

enum { ARBITRARY_NONZERO_VALUE = 0xFF };

void tst_edh_srv_message_3_setup_env(tst_edh_srv_message_3_env_t* env) {
  TEST_ASSERT_NOT_NULL_MESSAGE(env,
                               "Test environment pointer must not be NULL");

  env->session_dummy = 0;
  env->context_dummy = 0;
  env->response_dummy = 0;
  memset(env->request_payload, 0, sizeof(env->request_payload));
  memcpy(env->request_payload, CLEAN_MESSAGE_3_PAYLOAD,
         sizeof(CLEAN_MESSAGE_3_PAYLOAD));
  memset(env->response_payload, 0, sizeof(env->response_payload));
  env->request.base_data.session = (coap_session_t*)&env->session_dummy;
  env->request.base_data.edhoc_ctx = (struct edhoc_context*)&env->context_dummy;
  env->request.base_data.response = (coap_pdu_t*)&env->response_dummy;
  env->request.base_data.request_data.buffer = env->request_payload;
  env->request.base_data.request_data.length =
      sizeof(CLEAN_MESSAGE_3_PAYLOAD);
  env->request.message_3_extracted_fields = &env->extracted_fields;
  env->extracted_fields = (struct edhoc_extracted_fields){
      .buffer = env->request_payload,
      .buffer_size = sizeof(CLEAN_MESSAGE_3_PAYLOAD),
      .edhoc_message_ptr = env->request_payload,
      .edhoc_message_size = sizeof(CLEAN_MESSAGE_3_PAYLOAD),
  };
  env->response.buffer = env->response_payload;
  env->response.capacity = sizeof(env->response_payload);
  env->response.length = ARBITRARY_NONZERO_VALUE;
}

void tst_edh_srv_message_3_reset_response(com_response_buffer_t* response) {
  TEST_ASSERT_NOT_NULL_MESSAGE(response, "Response pointer must not be NULL");
  TEST_ASSERT_NOT_NULL_MESSAGE(response->buffer,
                               "Response payload pointer must not be NULL");
  response->length = ARBITRARY_NONZERO_VALUE;
  memset(response->buffer, 0, response->capacity);
}

void tst_edh_srv_message_3_assert_response_clean(
    const com_response_buffer_t* response) {
  if (!response || !response->buffer || !response->length) {
    return;
  }

  TEST_ASSERT_NOT_NULL_MESSAGE(response, "Response pointer must not be NULL");
  TEST_ASSERT_NOT_NULL_MESSAGE(response->buffer,
                               "Response payload pointer must not be NULL");
  TEST_ASSERT_EQUAL(ARBITRARY_NONZERO_VALUE, response->length);
  TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(
      0, response->buffer, response->capacity,
      "Handler should not modify the response buffer on invalid args");
}
