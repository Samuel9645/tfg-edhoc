/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 18/04/2026
 * @brief Tests for Message 3 processing.
 * @see [RFC 9528](https://datatracker.ietf.org/doc/html/rfc9528)
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 * @see [Helper Unity
 * scripts](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityHelperScriptsGuide.md)
 */

// ReSharper disable CppParameterMayBeConstPtrOrRef
#include <edhoc.h>
#include <string.h>
#include <unity.h>

#include "edhoc/server/handshake/message_3/srv_m3_process.h"
#include "edhoc/server/handshake/mocks/message_3/tst_srv_mock_edhoc_message_3_process.h"

enum { TST_SRV_EDHOC_HND_BUF_LEN = 256 };

static const uint8_t DUMMY_REQUEST_BUFFER[TST_SRV_EDHOC_HND_BUF_LEN] = {0};

static struct {
  struct edhoc_context context;
  struct srv_edhoc_message_3_request valid_request;
} env = {
    .valid_request = {.message_3 = {.bytes = DUMMY_REQUEST_BUFFER,
                                    .length = sizeof(DUMMY_REQUEST_BUFFER)}}};

void setUp(void) {
  tst_srv_edhoc_m3_reset_process_mock();
  env.valid_request.edhoc_context = &env.context;
  env.context = (struct edhoc_context){0};
}

void test_process_ok_for_valid_data(void) {
  tst_srv_edhoc_m3_set_process_ok();

  const struct srv_edhoc_message_3_process_result result =
      srv_edhoc_process_message_3(env.valid_request);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_PROCESS_OK, result.status);
}

void test_process_fails_on_invalid_data(void) {
  const struct srv_edhoc_message_3_request no_context = {
      .message_3 = env.valid_request.message_3};
  const struct srv_edhoc_message_3_request no_buffer = {.edhoc_context =
                                                            &env.context};
  const struct srv_edhoc_message_3_request empty_request = {
      .edhoc_context = &env.context,
      .message_3 = {.bytes = DUMMY_REQUEST_BUFFER, .length = 0}};

  const struct {
    const char* description;
    struct srv_edhoc_message_3_request request;
    enum srv_edhoc_message_3_process_status expected_status;
  } test_cases[] = {
      {"context is NULL", no_context,
       SRV_EDHOC_MSG3_PROCESS_ERR_NULL_EDHOC_CONTEXT},
      {"request buffer is NULL", no_buffer,
       SRV_EDHOC_MSG3_PROCESS_ERR_EMPTY_PARSED_MESSAGE_3},
      {"request payload is empty (len 0)", empty_request,
       SRV_EDHOC_MSG3_PROCESS_ERR_EMPTY_PARSED_MESSAGE_3},
  };

  for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
    tst_srv_edhoc_m3_reset_process_mock();

    const struct srv_edhoc_message_3_process_result result =
        srv_edhoc_process_message_3(test_cases[i].request);

    TEST_ASSERT_EQUAL_MESSAGE(test_cases[i].expected_status, result.status,
                              test_cases[i].description);
  }
}

void test_process_fails_on_library_errors(void) {
  tst_srv_edhoc_m3_set_process_failure();

  const struct srv_edhoc_message_3_process_result result =
      srv_edhoc_process_message_3(env.valid_request);

  TEST_ASSERT_EQUAL(SRV_EDHOC_MSG3_PROCESS_ERR_EDHOC_MESSAGE_3_PROCESS_FAILED,
                    result.status);
}
