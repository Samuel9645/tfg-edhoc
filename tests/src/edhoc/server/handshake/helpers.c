/**
 * @file helpers.c
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 30/03/2026
 * @brief Helper function implementations for EDHOC server handshake tests.
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */

#include "edhoc/server/handshake/helpers.h"

#include <edhoc.h>
#include <string.h>
#include <unity.h>

#include "edhoc/common/constants.h"

enum { ARBITRARY_NONZERO_VALUE = 0xFF };

static const struct edhoc_credentials DUMMY_TEST_CREDS = {0};

static void setup_valid_message_1_request_data(
    coap_session_t* session, coap_pdu_t* response, const uint8_t* payload,
    const size_t payload_len, edh_srv_hnd_m1_request_data_t* request_data) {
  request_data->base_data.session = session;
  request_data->base_data.edhoc_ctx = NULL;
  request_data->base_data.response = response;
  request_data->base_data.request_data.payload = payload;
  request_data->base_data.request_data.payload_length = payload_len;
  request_data->credentials = &DUMMY_TEST_CREDS;
}

static void setup_valid_message_1_response_data(
    uint8_t* buffer, const size_t buffer_capacity,
    com_response_buffer_t* response_data) {
  response_data->payload = buffer;
  response_data->payload_capacity = buffer_capacity;
  response_data->payload_length = buffer_capacity;
}

void override_test_request_payload(handshake_test_env_t* env,
                                   const uint8_t* new_payload,
                                   const size_t new_len) {
  env->request.base_data.request_data.payload = new_payload;
  env->request.base_data.request_data.payload_length = new_len;
}

void setup_testing_environment(handshake_test_env_t* env) {
  TEST_ASSERT_NOT_NULL_MESSAGE(env,
                               "Test environment pointer must not be NULL");

  env->session_dummy = 0;
  env->pdu_dummy = 0;
  memset(env->req_payload, 0, sizeof(env->req_payload));
  const uint8_t payload_with_prefix[] = {EDH_COM_CONST_CBOR_TRUE, 0x01, 0x02};
  memcpy(env->req_payload, payload_with_prefix, sizeof(payload_with_prefix));
  memset(env->res_payload, 0, sizeof(env->res_payload));
  env->res_written_len = ARBITRARY_NONZERO_VALUE;

  setup_valid_message_1_request_data(
      (coap_session_t*)&env->session_dummy, (coap_pdu_t*)&env->pdu_dummy,
      env->req_payload, sizeof(env->req_payload), &env->request);
  setup_valid_message_1_response_data(env->res_payload, env->res_written_len,
                                      &env->response);
}

void set_valid_message_1_payload(uint8_t* buffer, const size_t buffer_capacity,
                                 size_t* written_len) {
  TEST_ASSERT_NOT_NULL_MESSAGE(buffer, "Buffer pointer must not be NULL");
  TEST_ASSERT_NOT_NULL_MESSAGE(written_len,
                               "Written length pointer must not be NULL");
  const uint8_t payload_with_prefix[] = {EDH_COM_CONST_CBOR_TRUE, 0x01, 0x02};
  TEST_ASSERT_LESS_OR_EQUAL_size_t_MESSAGE(
      sizeof(payload_with_prefix), buffer_capacity,
      "Buffer capacity is too small for valid Message 1 payload");
  memcpy(buffer, payload_with_prefix, sizeof(payload_with_prefix));
  *written_len = sizeof(payload_with_prefix);
}

void reset_test_response(com_response_buffer_t* response_data) {
  TEST_ASSERT_NOT_NULL_MESSAGE(response_data,
                               "Test environment pointer must not be NULL");
  TEST_ASSERT_NOT_NULL_MESSAGE(response_data->payload,
                               "Test environment pointer must not be NULL");
  response_data->payload_length = ARBITRARY_NONZERO_VALUE;
  memset(response_data->payload, 0, response_data->payload_capacity);
}

void assert_response_untouched(const com_response_buffer_t* response) {
  if (!response || !response->payload || !response->payload_length)
    return;

  TEST_ASSERT_EQUAL_INT_MESSAGE(ARBITRARY_NONZERO_VALUE,
                                response->payload_length,
                                "Side-effect: payload_len was modified");

  TEST_ASSERT_EACH_EQUAL_UINT8_MESSAGE(
      0, response->payload, response->payload_capacity,
      "Side-effect: buffer content was modified");
}
