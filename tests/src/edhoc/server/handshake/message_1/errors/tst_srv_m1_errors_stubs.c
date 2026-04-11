/**
 * @file
 * @author Samuel Rodríguez <alu0101545714@ull.edu.es>
 * @since 5/04/2026
 * @brief Stubs to link to when testing the error handlers in Message 1
 * @see [Github Repository](https://github.com/Samuel9645/tfg-edhoc)
 */
// ReSharper disable CppParameterMayBeConstPtrOrRef
// ReSharper disable CppParameterMayBeConst
// since the real implementations expects non-const pointers to allow output
// parameters
// ReSharper disable CppParameterNamesMismatch
#include "edhoc/server/handshake/message_1/errors/tst_srv_m1_errors_stubs.h"

// TODO: rethink this, now is wrong

#include <edhoc.h>
#include <string.h>

#include "../../../../../../../include/edhoc/common/add_error/edhoc_error.h"
#include "unity.h"

const int SUCCESS_STUB_RESULT = EDHOC_SUCCESS;

bool tst_stub_get_suites_called = false;

enum { TST_SRV_M1_ERR_STUB_CIPHER_SUITES_SIZE = 8 };

static const int32_t
    TST_SRV_EDHOC_M1_ERROR_MOCK_BUFFER[TST_SRV_M1_ERR_STUB_CIPHER_SUITES_SIZE] =
        {0};
size_t tst_stub_get_cipher_suites_length = 0;

void tst_srv_m1_err_reset_stub_results(void) {
  tst_stub_get_suites_called = false;
  tst_stub_get_cipher_suites_length = 0;
  tst_stub_get_cipher_suites = TST_SRV_EDHOC_M1_ERROR_MOCK_BUFFER;
}

/* Stubbed version of libedhoc function */
int edhoc_error_get_cipher_suites(
    const struct edhoc_context* context, int32_t* cipher_suites,
    size_t cipher_suites_size,
    size_t* cipher_suites_length,  // NOLINT(*-non-const-parameter)
    int32_t* peer_cipher_suites,   // NOLINT(*-non-const-parameter)
    size_t peer_cipher_suites_size,
    size_t* peer_cipher_suites_length) {  // NOLINT(*-non-const-parameter)

  (void)context;
  (void)cipher_suites;
  (void)cipher_suites_size;
  (void)cipher_suites_length;
  (void)peer_cipher_suites;
  (void)peer_cipher_suites_length;
  (void)peer_cipher_suites_size;
  TEST_ASSERT_NOT_NULL_MESSAGE(
      cipher_suites, "Mock Error: Destination cipher suites buffer is NULL");
  TEST_ASSERT_NOT_NULL_MESSAGE(
      cipher_suites_length,
      "Mock Error: Output length pointer (cipher_suites_length) is NULL");
  if (tst_stub_get_cipher_suites_result != SUCCESS_STUB_RESULT) {
    return tst_stub_get_cipher_suites_result;
  }
  if (cipher_suites_size < tst_stub_get_cipher_suites_length) {
    return EDHOC_ERROR_BUFFER_TOO_SMALL;
  }
  memcpy(cipher_suites, tst_stub_get_cipher_suites,
         tst_stub_get_cipher_suites_length);
  *peer_cipher_suites_length = tst_stub_get_cipher_suites_length;
  return tst_stub_get_cipher_suites_result;
}

struct edhoc_error_info test_stub_captured_error_info = {0};

enum com_edhoc_add_edhoc_error_to_response_status
com_edhoc_add_edhoc_error_to_response(
    const struct edhoc_context* context,
    const struct edhoc_error_info* error_info,
    struct com_writable_buffer* response_data) {
  (void)context;
  (void)response_data;

  /* CAPTURE: Store the data passed by the handler so the test can inspect it */
  if (error_info != NULL) {
    test_stub_captured_error_info = *error_info;
  }

  return COM_EDHOC_ADD_ERROR_OK;
}

void tst_srv_m1_err_assert_response_contains_cipher_suites(
    const int32_t* expected_cipher_suites,
    const size_t expected_cipher_suites_length) {
  TEST_ASSERT_EQUAL_size_t_MESSAGE(expected_cipher_suites_length,
                                   test_stub_captured_error_info.total_entries,
                                   "The number of cipher suites captured in "
                                   "the error information is incorrect");

  /* 2. Verify the content of each cipher suite identifier */
  /* Note: In this context, text_string is being used as a generic pointer
     to the cipher suite array populated by the handler. */
  const int32_t* captured_suites =
      (const int32_t*)test_stub_captured_error_info.text_string;

  TEST_ASSERT_NOT_NULL_MESSAGE(captured_suites,
                               "The captured error information does not "
                               "contain a valid cipher suites pointer");

  for (size_t index = 0; index < expected_cipher_suites_length; index++) {
    TEST_ASSERT_EQUAL_INT32_MESSAGE(
        expected_cipher_suites[index], captured_suites[index],
        "A mismatch was detected in the captured cipher suite identifiers");
  }
}