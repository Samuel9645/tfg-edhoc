#include <stdbool.h>
#include <stdint.h>
#include <unity.h>

#include "coap/common/status.h"
#include "internal/edhoc_dispatch_internal.h"

/*
 * Stubs required by COAP_SERVER_EDHOC_DISPATCH_DEFAULT_DEPS symbol in
 * edhoc_dispatch.c. Tests call only the injected seam and never use these.
 */
coap_status_result_t coap_shared_validate_edhoc_request(
    const coap_pdu_t* request, content_format_edhoc_values_t expected_format,
    const uint8_t** payload, size_t* payload_len) {
  (void)request;
  (void)expected_format;
  (void)payload;
  (void)payload_len;
  return COAP_STATUS_ERROR;
}

coap_status_result_t coap_shared_add_edhoc_response_options(
    coap_pdu_t* response, content_format_edhoc_values_t content_format) {
  (void)response;
  (void)content_format;
  return COAP_STATUS_ERROR;
}

bool coap_shared_is_properly_formatted_message_1(const uint8_t* payload,
                                                 size_t payload_len) {
  (void)payload;
  (void)payload_len;
  return false;
}

bool coap_shared_is_properly_formatted_message_3(
    const uint8_t* request_payload, size_t request_len,
    const struct edhoc_context* edhoc_ctx,
    struct edhoc_extracted_fields* extracted_fields) {
  (void)request_payload;
  (void)request_len;
  (void)edhoc_ctx;
  (void)extracted_fields;
  return false;
}

coap_pdu_code_t server_edhoc_handle_message_1(
    const server_edhoc_message_1_request_data_t* request_data,
    coap_response_data_t* response_data) {
  (void)request_data;
  (void)response_data;
  return COAP_RESPONSE_CODE_INTERNAL_ERROR;
}

coap_pdu_code_t server_edhoc_handle_message_3(
    const server_edhoc_message_3_request_data_t* request_data,
    coap_response_data_t* response_data) {
  (void)request_data;
  (void)response_data;
  return COAP_RESPONSE_CODE_INTERNAL_ERROR;
}

coap_status_result_t coap_shared_add_response_payload(coap_pdu_t* response,
                                                      const uint8_t* payload,
                                                      size_t payload_len) {
  (void)response;
  (void)payload;
  (void)payload_len;
  return COAP_STATUS_ERROR;
}

typedef struct {
  coap_pdu_code_t last_code;
  size_t set_code_calls;
} fake_response_t;

typedef struct {
  void* app_data;
} fake_session_t;

static bool g_add_payload_called = false;

static coap_status_result_t fake_validate_ok(
    const coap_pdu_t* request, content_format_edhoc_values_t expected_format,
    const uint8_t** payload, size_t* payload_len) {
  static const uint8_t message_1_payload[] = {0xf5, 0x01};
  (void)request;
  (void)expected_format;
  *payload = message_1_payload;
  *payload_len = sizeof(message_1_payload);
  return COAP_STATUS_SUCCESS;
}

static coap_status_result_t fake_add_options_fail(
    coap_pdu_t* response, content_format_edhoc_values_t content_format) {
  (void)response;
  (void)content_format;
  return COAP_STATUS_ERROR;
}

static coap_status_result_t fake_add_options_ok(
    coap_pdu_t* response, content_format_edhoc_values_t content_format) {
  (void)response;
  (void)content_format;
  return COAP_STATUS_SUCCESS;
}

static bool fake_is_message_1(const uint8_t* payload, size_t payload_len) {
  (void)payload;
  (void)payload_len;
  return true;
}

static bool fake_is_message_3(const uint8_t* request_payload,
                              size_t request_len,
                              const struct edhoc_context* edhoc_ctx,
                              struct edhoc_extracted_fields* extracted_fields) {
  (void)request_payload;
  (void)request_len;
  (void)edhoc_ctx;
  (void)extracted_fields;
  return false;
}

static coap_pdu_code_t fake_handle_message_1_internal_error(
    const server_edhoc_message_1_request_data_t* request_data,
    coap_response_data_t* response_data) {
  (void)request_data;
  *response_data->payload_len = 0;
  return COAP_RESPONSE_CODE_INTERNAL_ERROR;
}

static coap_pdu_code_t fake_handle_message_1_changed_with_payload(
    const server_edhoc_message_1_request_data_t* request_data,
    coap_response_data_t* response_data) {
  (void)request_data;
  response_data->payload[0] = 0xaa;
  *response_data->payload_len = 1;
  return COAP_RESPONSE_CODE_CHANGED;
}

static coap_pdu_code_t fake_handle_message_3(
    const server_edhoc_message_3_request_data_t* request_data,
    coap_response_data_t* response_data) {
  (void)request_data;
  (void)response_data;
  return COAP_RESPONSE_CODE_INTERNAL_ERROR;
}

static coap_status_result_t fake_add_payload_ok(coap_pdu_t* response,
                                                const uint8_t* payload,
                                                size_t payload_len) {
  (void)response;
  (void)payload;
  (void)payload_len;
  g_add_payload_called = true;
  return COAP_STATUS_SUCCESS;
}

static coap_status_result_t fake_add_payload_fail(coap_pdu_t* response,
                                                  const uint8_t* payload,
                                                  size_t payload_len) {
  (void)response;
  (void)payload;
  (void)payload_len;
  g_add_payload_called = true;
  return COAP_STATUS_ERROR;
}

static void* fake_get_session_app_data(const coap_session_t* session) {
  const fake_session_t* fake_session = (const fake_session_t*)session;
  return fake_session->app_data;
}

static void fake_set_response_code(coap_pdu_t* response, coap_pdu_code_t code) {
  fake_response_t* fake_response = (fake_response_t*)response;
  fake_response->last_code = code;
  fake_response->set_code_calls++;
}

static coap_server_edhoc_dispatch_deps_t make_base_deps(void) {
  return (coap_server_edhoc_dispatch_deps_t){
      .validate_edhoc_request = fake_validate_ok,
      .add_edhoc_response_options = fake_add_options_ok,
      .is_message_1 = fake_is_message_1,
      .is_message_3 = fake_is_message_3,
      .handle_message_1 = fake_handle_message_1_internal_error,
      .handle_message_3 = fake_handle_message_3,
      .add_response_payload = fake_add_payload_ok,
      .get_session_app_data = fake_get_session_app_data,
      .set_response_code = fake_set_response_code,
  };
}

void setUp(void) {}

void tearDown(void) {}

static void test_sets_internal_error_when_adding_options_fails(void) {
  fake_session_t fake_session = {.app_data = NULL};
  fake_response_t fake_response = {.last_code = COAP_EMPTY_CODE,
                                   .set_code_calls = 0};
  const coap_pdu_t* fake_request = (const coap_pdu_t*)&fake_session;

  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.add_edhoc_response_options = fake_add_options_fail;

  coap_server_dispatch_edhoc_post_with_deps((coap_session_t*)&fake_session,
                                            fake_request,
                                            (coap_pdu_t*)&fake_response, &deps);

  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR, fake_response.last_code);
  TEST_ASSERT_EQUAL_UINT32(1u, (uint32_t)fake_response.set_code_calls);
}

static void test_propagates_handler_code_when_payload_not_added(void) {
  fake_session_t fake_session = {.app_data = NULL};
  fake_response_t fake_response = {.last_code = COAP_EMPTY_CODE,
                                   .set_code_calls = 0};
  const coap_pdu_t* fake_request = (const coap_pdu_t*)&fake_session;

  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.handle_message_1 = fake_handle_message_1_internal_error;

  coap_server_dispatch_edhoc_post_with_deps((coap_session_t*)&fake_session,
                                            fake_request,
                                            (coap_pdu_t*)&fake_response, &deps);

  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR, fake_response.last_code);
  TEST_ASSERT_EQUAL_UINT32(1u, (uint32_t)fake_response.set_code_calls);
}

static void test_payload_add_failure_overrides_handler_code(void) {
  fake_session_t fake_session = {.app_data = NULL};
  fake_response_t fake_response = {.last_code = COAP_EMPTY_CODE,
                                   .set_code_calls = 0};
  const coap_pdu_t* fake_request = (const coap_pdu_t*)&fake_session;

  g_add_payload_called = false;

  coap_server_edhoc_dispatch_deps_t deps = make_base_deps();
  deps.handle_message_1 = fake_handle_message_1_changed_with_payload;
  deps.add_response_payload = fake_add_payload_fail;

  coap_server_dispatch_edhoc_post_with_deps((coap_session_t*)&fake_session,
                                            fake_request,
                                            (coap_pdu_t*)&fake_response, &deps);

  TEST_ASSERT_TRUE(g_add_payload_called);
  TEST_ASSERT_EQUAL(COAP_RESPONSE_CODE_INTERNAL_ERROR, fake_response.last_code);
  TEST_ASSERT_EQUAL_UINT32(1u, (uint32_t)fake_response.set_code_calls);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_sets_internal_error_when_adding_options_fails);
  RUN_TEST(test_propagates_handler_code_when_payload_not_added);
  RUN_TEST(test_payload_add_failure_overrides_handler_code);
  return UNITY_END();
}
