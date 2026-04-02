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
  env->response.buffer = env->response_payload;
  env->response.capacity = sizeof(env->response_payload);
  tst_edh_srv_message_3_reset_response(&env->response);
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

com_response_buffer_t tst_invalid_message_3_response(void) {
  return (com_response_buffer_t){
      .buffer = NULL, .capacity = 0, .length = ARBITRARY_NONZERO_VALUE};
}

edh_srv_message_3_request_t tst_empty_message_3_request(void) {
  return (edh_srv_message_3_request_t){0};
}

static const uint8_t TST_CANARY_PATTERN = 0xAA;

void tst_edh_srv_message_3_reset_response(com_response_buffer_t* response) {
  TEST_ASSERT_NOT_NULL(response);
  TEST_ASSERT_NOT_NULL(response->buffer);

  response->length = ARBITRARY_NONZERO_VALUE;
  memset(response->buffer, TST_CANARY_PATTERN, response->capacity);
}

static void assert_length_is_untouched(const com_response_buffer_t* response) {
  TEST_ASSERT_EQUAL_UINT32(ARBITRARY_NONZERO_VALUE, response->length);
}

static void assert_buffer_is_untouched(const com_response_buffer_t* response) {
  TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(
      TST_CANARY_PATTERN, response->buffer, response->capacity,
      "Handler modified the response buffer when it should have failed early.");
}

static bool buffer_provided(const com_response_buffer_t* response) {
  return response->buffer != NULL;
}

void tst_edh_srv_message_3_assert_response_clean(
    const com_response_buffer_t* response) {
  TEST_ASSERT_NOT_NULL_MESSAGE(response, "Response struct pointer is NULL");
  if (buffer_provided(response)) {
    assert_length_is_untouched(response);
    assert_buffer_is_untouched(response);
  } else {
    assert_length_is_untouched(response);
  }
}
