/**
 * @file helpers.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Helper function implementations for EDHOC server handshake tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

// ReSharper disable CppDFAConstantParameter
#include "../../../../../../include/edhoc/server/handshake/message_1/handler/helpers.h"

#include <edhoc.h>
#include <string.h>
#include <unity.h>

enum { ARBITRARY_NONZERO_VALUE = 0xFF };

static const struct edhoc_credentials DUMMY_TEST_CREDS = {0};
static const uint8_t CLEAN_MESSAGE_1_PAYLOAD[] = {0x01, 0x02};
static const size_t CLEAN_MESSAGE_1_PAYLOAD_SIZE =
    sizeof(CLEAN_MESSAGE_1_PAYLOAD);

static void set_request_payload(const uint8_t* payload,
                                const size_t payload_len,
                                edh_srv_message_1_request_t* request_data) {
  request_data->payload.buffer = payload;
  request_data->payload.length = payload_len;
}

static void set_response_payload(const uint8_t* payload,
                                 const size_t payload_len,
                                 com_response_buffer_t* response_data) {
  response_data->buffer = (uint8_t*)payload;
  response_data->capacity = payload_len;
  response_data->length = 0;
}

void tst_edh_override_message_1_handler_request(
    tst_message_1_handler_env_t* env, const uint8_t* new_payload,
    const size_t new_len) {
  set_request_payload(new_payload, new_len, &env->request);
}

void tst_edh_setup_message_1_handler_env(tst_message_1_handler_env_t* env) {
  TEST_ASSERT_NOT_NULL_MESSAGE(env,
                               "Test environment pointer must not be NULL");

  memset(env->req_payload, 0, sizeof(env->req_payload));
  memcpy(env->req_payload, CLEAN_MESSAGE_1_PAYLOAD,
         sizeof(CLEAN_MESSAGE_1_PAYLOAD));
  memset(env->res_payload, 0, sizeof(env->res_payload));
  env->res_written_len = ARBITRARY_NONZERO_VALUE;
  set_request_payload(env->req_payload, sizeof(env->req_payload),
                      &env->request);
  set_response_payload(env->res_payload, sizeof(env->res_payload),
                       &env->response);
  env->request.credentials = &DUMMY_TEST_CREDS;
}

void tst_edh_set_valid_message_1_request(uint8_t* buffer,
                                         const size_t buffer_capacity,
                                         size_t* written_len) {
  TEST_ASSERT_NOT_NULL_MESSAGE(buffer, "Buffer pointer must not be NULL");
  TEST_ASSERT_NOT_NULL_MESSAGE(written_len,
                               "Written length pointer must not be NULL");
  TEST_ASSERT_LESS_OR_EQUAL_size_t_MESSAGE(
      CLEAN_MESSAGE_1_PAYLOAD_SIZE, buffer_capacity,
      "Buffer capacity is too small for valid Message 1 payload");

  memcpy(buffer, CLEAN_MESSAGE_1_PAYLOAD, CLEAN_MESSAGE_1_PAYLOAD_SIZE);
  *written_len = CLEAN_MESSAGE_1_PAYLOAD_SIZE;
}

void tst_edh_reset_message_1_response(com_response_buffer_t* response_data) {
  TEST_ASSERT_NOT_NULL_MESSAGE(response_data,
                               "Test environment pointer must not be NULL");
  TEST_ASSERT_NOT_NULL_MESSAGE(response_data->buffer,
                               "Test environment pointer must not be NULL");

  response_data->length = ARBITRARY_NONZERO_VALUE;
  memset(response_data->buffer, 0, response_data->capacity);
}

void tst_edh_assert_message_1_handler_response_clean(
    const com_response_buffer_t* response) {
  if (!response || !response->buffer || !response->length)
    return;

  TEST_ASSERT_EQUAL_INT_MESSAGE(ARBITRARY_NONZERO_VALUE, response->length,
                                "Side-effect: payload_len was modified");

  TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(
      0, response->buffer, response->capacity,
      "Side-effect: buffer content was modified");
}
