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
static const size_t CLEAN_MESSAGE_3_PAYLOAD_SIZE =
    sizeof(CLEAN_MESSAGE_3_PAYLOAD);

enum { ARBITRARY_NONZERO_VALUE = 0xFF };

static void set_response_payload(const uint8_t* payload,
                                 const size_t payload_len,
                                 com_response_buffer_t* response_data) {
  response_data->buffer = (uint8_t*)payload;
  response_data->capacity = payload_len;
  response_data->length = 0;
}

void tst_edh_srv_message_3_setup_env(tst_edh_srv_message_3_env_t* env) {
  TEST_ASSERT_NOT_NULL_MESSAGE(env,
                               "Test environment pointer must not be NULL");

  memset(env->request_payload, 0, sizeof(env->request_payload));
  memcpy(env->request_payload, CLEAN_MESSAGE_3_PAYLOAD,
         CLEAN_MESSAGE_3_PAYLOAD_SIZE);
  memset(env->response_payload, 0, sizeof(env->response_payload));
  env->response_written_len = ARBITRARY_NONZERO_VALUE;
  set_response_payload(env->response_payload, sizeof(env->response_payload),
                       &env->response);
  env->context_dummy = 0;
  env->extracted_fields = (struct edhoc_extracted_fields){
      .buffer = env->request_payload,
      .buffer_size = sizeof(CLEAN_MESSAGE_3_PAYLOAD),
      .edhoc_message_ptr = env->request_payload,
      .edhoc_message_size = sizeof(CLEAN_MESSAGE_3_PAYLOAD),
  };
  env->request = (edh_srv_message_3_request_t){
      .edhoc_ctx = (struct edhoc_context*)&env->context_dummy,
      .message_3_extracted_fields = &env->extracted_fields,
  };
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
